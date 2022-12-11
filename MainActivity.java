package com.wz.musicgame;

import java.util.*;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.ImageView;

import com.wz.jnidriver.JNIDriver;
import com.wz.jnidriver.JNIListener;

public class MainActivity extends Activity implements JNIListener {
    static JNIDriver driver;
    static byte[] ledData = {0, 0, 0, 0, 0, 0, 0};
    static byte[] notes = new byte[500];
    
    static ImageView arrowBoard;
    static TextView gameMessage;
    
    static final int WAITING = 0;
    static final int LOCKED = 1;
    static final int OPEN = 2;
    
    static final int DO = 0;
    static final int MI = 4;
    static final int SO = 7;
    static final int HIGH_DO = 12;
    static final int CENTER = 1;
    
    static final String WAITING_MESSAGE = "【CENTER】 버튼을 눌러 게임을 시작해 주세요.";
    static final String START_MESSAGE = "게임을 시작합니다!";
    static final String INTRODUCE_MESSAGE_1 = "【HOW TO PLAY】\n1. 멜로디가 연주되면 주의 깊게 들어 주세요."
    static final String INTRODUCE_MESSAGE_2 = "【HOW TO PLAY】\n2. 멜로디가 끝나면 연주된 멜로디를 방향키를 눌러 똑같이 연주해 주세요.";
    static final String INTRODUCE_MESSAGE_3 = "【HOW TO PLAY】\n3. 멜로디를 잘못 연주해 LIFE가 모두 소진되면, 게임은 끝납니다!";
    static final String INTRODUCE_MESSAGE_4 = "이제 게임을 시작하겠습니다!";
    static final String LISTEN_MESSAGE = "멜로디가 연주되고 있습니다! 주의 깊게 들어 주세요.";
    static final String PLAY_MESSAGE = "이제, 들려드린 멜로디를 똑같이 연주해 주세요!";
    static final String CORRECT_MESSAGE = "맞았습니다! 다음 라운드로 이동합니다.";
    static final String ERROR_MESSAGE = "게임에 필요한 드라이버를 불러오는 데 실패하였습니다. 앱을 재시작해 주세요.";
    
    static ScoreThread scoreThread;
    static boolean isScoreThreadRunning = false;
    static int buttonLocker = NOT_STARTED;
    static int score;
    static int life;
    static int round;
    static int guessIndex;
    static int firstRun = true;
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        driver = new JNIDriver();
        arrowBoard = (ImageView) findViewById(R.id.arrowBoard);
        gameMessage = (TextView) findViewById(R.id.gameMessage);
        
        scoreThread = new ScoreThread();
        scoreThread.run();
        
        waitingPhase();
    }
    
    public static void waitingPhase() {
        buttonLocker = WAITING;
        
        driver.clearLCDText();
        driver.setLCDText("PRESS CENTER", 0);
        driver.setLCDText("TO START", 1);
        gameMessage.setText(WAITING_MESSAGE);
    }
    
    public static void startPhase() {
        score = 0;
        life = 3;
        round = 1;
        buttonLocker = LOCKED;
        
        driver.clearLCDText();
        driver.setLCDText("GAME START", 0);
        
        delay(3000);
        
        if (firstRun) {
            firstRun = false;
            introducePhase();
        }
        
        isScoreThreadRunning = true;
        listenPhase();
    }
    
    public static void introducePhase() {
        gameMessage.setText(INTRODUCE_MESSAGE_1);
        delay(3000);
        gameMessage.setText(INTRODUCE_MESSAGE_2);
        delay(3000);
        gameMessage.setText(INTRODUCE_MESSAGE_3);
        delay(3000);
        gameMessage.setText(INTRODUCE_MESSAGE_4);
        delay(3000);
    }
    
    public static void roundAnnouncePhase() {
        round += 1;
        
        driver.clearLCDText();
        driver.setLCDText("ROUND " + round, 0);
        driver.setLCDText("LIFE " + life, 1);
        gameMessage.setText(round + " 단계");
        
        createRandomNote(round);
        
        delay(2000);
        listenPhase();
    }
    
    public static void listenPhase() {
        driver.clearLCDText();
        driver.setLCDText("ROUND " + round, 0);
        driver.setLCDText("LIFE " + life, 1);
        gameMessage.setText(LISTEN_MESSAGE);
        
        for (int i = 1; i <= round; i++) {
            byte[] ledBit = {0, 0, 0, 0, 0, 0, 0};
            ledBit[getConvertedLEDValueFromNote(notes[i])] = 1;
            
            displayLED(ledBit);
            playNote(notes[i]);
        }
        
        playPhase();
    }
    
    public static void playPhase() {
        driver.clearLCDText();
        driver.setLCDText("ROUND " + round, 0);
        driver.setLCDText("LIFE " + life, 1);
        gameMessage.setText(PLAY_MESSAGE);
        
        guessIndex = 1;
        buttonLocker = OPEN;
    }
    
    public static void correctAnswerPhase() {
        buttonLocker = LOCKED;
        
        driver.setLCDText("GREAT JOB", 0);
        driver.setLCDText("LIFE " + life, 1);
        gameMessage.setText("정답입니다! 다음 라운드로 넘어가겠습니다.");
        
        delay(2000);
        
        roundAnnouncePhase();
    }
    
    public static void wrongAnswerPhase() {
        buttonLocker = LOCKED;
        life -= 1;
        
        driver.setLCDText("WRONG MELODY", 0);
        driver.setLCDText("LIFE " + life, 1);
        gameMessage.setText("틀렸습니다! 남은 LIFE는 " + life + " 개 입니다.");
        driver.setVibrator(1);
        
        delay(2000);
        
        driver.setVibrator(0);
        
        if (life <= 0) {
            gameOverPhase();
        } else {
            tryAgainPhase();
        }
    }
    
    public static void tryAgainPhase() {
        driver.setLCDText("LETS TRY AGAIN", 0);
        driver.setLCDText("LIFE " + life, 1);
        gameMessage.setText("다시 멜로디를 들려드리겠습니다.");
            
        delay(3000);
            
        listenPhase();
    }
    
    public static void gameOverPhase() {
        driver.setLCDText("GAME OVER", 0);
        driver.setLCDText("NICE PLAY", 1);
        gameMessage.setText("【GAME OVER】\n" + round + " 라운드에서 탈락하셨으며,\n" + score + " 점을 기록하셨습니다.");
            
        delay(7000);
        
        isScoreThreadRunning = true;
        waitingPhase();
    }
    
    @Override
    public void onPause() {
        driver.close();
        super.onPause();
    }
    
    @Override
    protected void onResume() {
        if (driver.open() < 0) {
            gameMessage.setText(ERROR_MESSAGE);
        }
        super.onResume();
    }
    
    public static void delay(int miliseconds) {
        try {
            Thread.sleep(miliseconds);
        } catch (InterruptedException e) { }
    }
    
    public static void createRandomNote(int selectedIndex) {
        byte[] sampleNotes = {DO, MI, SO, HIGH_DO};
        int randomIndex = (int) (Math.random() * 4);
        
        notes[selectedIndex] = sampleNotes[randomIndex];
    }
    
    public static void judgeSingleNote(int currentGuess) {
        if (guessIndex > round) return;
        
        if (currentGuess == notes[guessIndex]) {
            score += round;
            guessIndex += 1;
            
            byte[] ledBit = {0, 0, 0, 0, 0, 0, 0};
            ledBit[getConvertedLEDValueFromNote(notes[guessIndex - 1])] = 1;
            
            displayLED(ledBit);
            playNote(notes[guessIndex - 1]);
            
            if (guessIndex > round) {
                correctAnswerPhase();
            }
        } else {
            wrongAnswerPhase();
        }
    }
    
    public static int getConvertedLEDValueFromNote(int noteValue) {
        switch (noteValue) {
            case DO:
                return 7;
            case MI:
                return 6;
            case SO:
                return 5;
            case HIGH_DO:
                return 4;
            default:
                return -1;
        }
    }
    
    public class ScoreThread extends Thread {
        @Override
        public void run() {
            super.run();
            
            while (isScoreThreadRunning) {
                byte[] data = {0, 0, 0, 0, 0, 0};
                
                data[0] = (byte) (score / 100000 % 10);
                data[1] = (byte) (score / 10000 % 10);
                data[2] = (byte) (score / 1000 % 10);
                data[3] = (byte) (score / 100 % 10);
                data[4] = (byte) (score / 10 % 10);
                data[5] = (byte) (score % 10);
                
                driver.displaySegment(data);
            }
        }
    }
    
    public Handler pushButtonHandler = new Handler() {
        public void handleMessage(Message message) {
            switch (message.arg1) {
                case CENTER:
                    if (buttonLocker == NOT_STARTED) {
                        startPhase();
                    }
                    break;
                case DO:
                case MI:
                case SO:
                case HIGH_DO:
                    if (buttonLocker == OPEN) {
                        judgeSingleNote(message.arg1);
                    }
                    break;
            }
        }
    }
}