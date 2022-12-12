#include <fcntl.h>
#include <jni.h>
#include <string.h>
#include <sys/ioctl.h>

#define FAIL -1
#define SUCCESS 1

#define DO 0
#define MI 4
#define SO 7
#define HIGH_DO 12
#define CENTER 1

#define UP_TEXT "Up"
#define DOWN_TEXT "Down"
#define LEFT_TEXT "Left"
#define RIGHT_TEXT "Right"
#define CENTER_TEXT "Center"

#define LED_PATH "/dev/sm9s5422_led"
#define VIBRATOR_PATH "/dev/sm9s5422_perivib"
#define PIEZO_PATH "/dev/sm9s5422_piezo"
#define SEGMENT_PATH "/dev/sm9s5422_segment"
#define PUSHBUTTON_PATH "/dev/sm9s5422_interrupt"
#define TEXTLCD_PATH "/dev/sm9s5422_textlcd"

#define TEXTLCD_BASE 0x56
#define TEXTLCD_FUNCTION_SET _IO(TEXTLCD_BASE, 0x31)

#define TEXTLCD_DISPLAY_ON _IO(TEXTLCD_BASE, 0x32)
#define TEXTLCD_DISPLAY_OFF _IO(TEXTLCD_BASE, 0x33)
#define TEXTLCD_DISPLAY_CURSOR_ON _IO(TEXTLCD_BASE, 0x34)
#define TEXTLCD_DISPLAY_CURSOR_OFF _IO(TEXTLCD_BASE, 0x35)

#define TEXTLCD_CURSOR_SHIFT_RIGHT _IO(TEXTLCD_BASE, 0x36)
#define TEXTLCD_CURSOR_SHIFT_LEFT _IO(TEXTLCD_BASE, 0x37)

#define TEXTLCD_ENTRY_MODE_SET _IO(TEXTLCD_BASE, 0x38)
#define TEXTLCD_RETURN_HOME _IO(TEXTLCD_BASE, 0x39)
#define TEXTLCD_CLEAR _IO(TEXTLCD_BASE, 0x3a)

#define TEXTLCD_DD_ADDRESS_1 _IO(TEXTLCD_BASE, 0x3b)
#define TEXTLCD_DD_ADDRESS_2 _IO(TEXTLCD_BASE, 0x3c)
#define TEXTLCD_WRITE_BYTE _IO(TEXTLCD_BASE, 0x3d)

int led_fd = -1;
int vibrator_fd = -1;
int piezo_fd = -1;
int segment_fd = -1;
int pushbutton_fd = -1;
int textlcd_fd = -1;

JNIEXPORT jint JNICALL Java_com_wz_jnidriver_JNIDriver_openDrivers(JNIEnv *env, jclass class) {
    led_fd = open(LED_PATH, O_WRONLY);
    vibrator_fd = open(VIBRATOR_PATH, O_WRONLY);
    piezo_fd = open(PIEZO_PATH, O_WRONLY);
    segment_fd = open(SEGMENT_PATH, O_WRONLY);
    pushbutton_fd = open(PUSHBUTTON_PATH, O_RDONLY);
    textlcd_fd = open(TEXTLCD_PATH, O_WRONLY);

    if (piezo_fd < 0 || vibrator_fd < 0 || segment_fd < 0) return FAIL;
    return SUCCESS;
}

JNIEXPORT void JNICALL Java_com_wz_jnidriver_JNIDriver_closeDrivers(JNIEnv *env, jclass class) {
    if (led_fd > 0) close(led_fd);
    if (vibrator_fd > 0) close(vibrator_fd);
    if (piezo_fd > 0) close(piezo_fd);
    if (segment_fd > 0) close(segment_fd);
    if (pushbutton_fd > 0) close(pushbutton_fd);
    if (textlcd_fd > 0) close(textlcd_fd);
}

JNIEXPORT void JNICALL Java_com_wz_jnidriver_JNIDriver_writeLED(JNIEnv *env, jobject obj, jbyteArray arr, jint count) {
    if (led_fd < 0) return;

    jbyte led_chars = (*env)->GetByteArrayElements(env, arr, 0);
    write(led_fd, (unsigned char *)led_chars, count);
    (*env)->ReleaseByteArrayElements(env, arr, led_chars, 0);
}

JNIEXPORT void JNICALL Java_com_wz_jnidriver_JNIDriver_writeVibrator(JNIEnv *env, jobject obj, jchar char_command) {
    if (vibrator_fd < 0) return;

    int command = (int)char_command;
    write(vibrator_fd, &command, sizeof(command));
}

JNIEXPORT void JNICALL Java_com_wz_jnidriver_JNIDriver_writePiezo(JNIEnv *env, jobject obj, jchar char_note) {
    if (piezo_fd < 0) return;

    int note = (int)char_note;
    write(piezo_fd, &note, sizeof(note));
}

JNIEXPORT void JNICALL Java_com_wz_jnidriver_JNIDriver_writeSegment(JNIEnv *env, jobject obj, jbyteArray arr, jint duration) {
    if (segment_fd < 0) return;

    jbyte *segment_chars = (*env)->GetByteArrayElements(env, arr, 0);
    write(segment_fd, (unsigned char *)segment_chars, duration);
    (*env)->ReleaseByteArrayElements(env, arr, segment_chars, 0);
}

JNIEXPORT jint JNICALL Java_com_wz_jnidriver_JNIDriver_getInterrupt(JNIEnv *env, jobject obj) {
    char read_data[100];
    int read_flag = read(pushbutton_fd, &read_data, 100);

    if (read_flag < 0) return -1;

    if (strcmp(read_data, CENTER_TEXT) == 0) return CENTER;
    if (strcmp(read_data, DOWN_TEXT) == 0) return DO;
    if (strcmp(read_data, LEFT_TEXT) == 0) return MI;
    if (strcmp(read_data, RIGHT_TEXT) == 0) return SO;
    if (strcmp(read_data, UP_TEXT) == 0) return HIGH_DO;

    return -1;
}

JNIEXPORT void JNICALL Java_com_wz_jnidriver_JNIDriver_writeLCDLine(JNIEnv *env, jclass class, jstring text, jint len, jint slot) {
    if (textlcd_fd < 0) return;

    jboolean iscopy;
    int i = 0;
    const char *text_utf = (*env)->GetStringUTFChars(env, text, &iscopy);

    if (slot == 0) {
        ioctl(textlcd_fd, TEXTLCD_DD_ADDRESS_1, NULL);
    } else {
        ioctl(textlcd_fd, TEXTLCD_DD_ADDRESS_2, NULL);
    }

    for (i = 0; i < len; i++) {
        ioctl(textlcd_fd, TEXTLCD_WRITE_BYTE, text_utf[i]);
    }

    (*env)->ReleaseStringUTFChars(env, text, text_utf);
}

JNIEXPORT void JNICALL Java_com_wz_jnidriver_JNIDriver_clearLCD(JNIEnv *env, jclass class) {
    if (textlcd_fd < 0) return;

    ioctl(textlcd_fd, TEXTLCD_CLEAR, NULL);
}