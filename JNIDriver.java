package com.wz.jnidriver;

public class JNIDriver implements JNIListener {
    private boolean isConnected = false;
    private JNIListener mainActivityTosser;
    private PushButtonThread pushButtonThread;
    final int SUCCESS = 1;
    final int FAIL = -1;
    
    static {
        System.loadLibrary("JNICDriver");
    }
    
    private native static int openDrivers();
    private native static void closeDrivers();
    private native static void writeLED(byte[] arr, int count);
    private native static void writeVibrator(char command);
    private native static void writePiezo(char char_note);
    private native static void writeSegment(byte[] arr, int duration);
    private native int getInterrupt();
    private native static void writeLCDLine(String text, int len, int slot);
    private native static void clearLCD();

    public JNIDriver() {
        isConnected = false;
    }
    
    public int open() {
        if (isConnected) return FAIL;
        if (openDrivers() < 0) return FAIL;
        
        isConnected = true;
        pushButtonThread = new PushButtonThread();
        pushButtonThread.start();
        
        return SUCCESS;
    }
    
    public void displayLED(byte[] data) {
        if (!isConnected) return;
        
        writeLED(data, data.length);
    }
    
    public void setVibrator(int command) {
        if (!isConnected) return;
        
        writeVibrator((char) command);
    }
    
    public void playNote(int note) {
        if (!isConnected || note == -1) return;
        
        writePiezo((char) note);
    }
    
    public void displaySegment(byte[] data) {
	    if (!isConnected) return;
		
        writeSegment(data, data.length);
	}
    
    @Override
    public void onReceive(int value) {
        if (mainActivityTosser != null) {
            mainActivityTosser.onReceive(value);
        }
    }
    
    public void setListener(JNIListener listener) {
        mainActivityTosser = listener;
    }
    
    public void setLCDText(String text, int slot) {
        if (!isConnected) return;
        
        writeLCDLine(text, text.length(), slot);
    }
    
    public void clearLCDText() {
        if (!isConnected) return;
        
        clearLCD();
    }
    
    public void close() {
        if (isConnected) {
            return;
        }
        
        closeDrivers();
    }
    
    public void finalize() throws Throwable {
        close();
        super.finalize();
    }
    
    
    private class PushButtonThread extends Thread {
        @Override
        public void run() {
            super.run();
        
            try {
	            while (true) {
	                try {
	                    onReceive(getInterrupt());
	                    Thread.sleep(100);
	                } catch (InterruptedException e) {
	                    e.printStackTrace();
	                }
	            }
            } catch (Exception e) { }
        }
    }
}
