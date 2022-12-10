#include <fcntl.h>
#include <jni.h>
#include <string.h>
#include <sys/ioctl.h>

#define FAIL -1
#define SUCCESS 1

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

int led_fd, vibrator_fd, piezo_fd, segment_fd, textlcd_fd;

JNIEXPORT jint JNICALL Java_com_wz_jnidriver_JNIDriver_openDrivers(JNIEnv *env, jclass class) {
    led_fd = open(LED_PATH, O_WRONLY);
    vibrator_fd = open(VIBRATOR_PATH, O_WRONLY);
    piezo_fd = open(PIEZO_PATH, O_WRONLY);
    segment_fd = open(SEGMENT_PATH, O_WRONLY);
    pushbutton_fd = open(PUSHBUTTON_PATH, O_RDONLY);
    textlcd_fd = open(TEXTLCD_PATH, O_WRONLY);

    if (led_fd < 0 || vibrator_fd < 0 || piezo_fd < 0 || segment_fd < 0 || pushbutton_fd < 0 || textlcd_fd < 0) return FAIL;
    return SUCCESS;
}

JNIEXPORT void JNICALL Java_com_wz_jnidriver_JNIDriver_closeDrivers(JNIEnv *env, jclass class) {
    if (led_fd > 0) close(led_fd);
    if (vibrator_fd > 0) close(vibrator_fd);
    if (piezo_fd > 0) close(piezo_fd);
    if (segment_fd > 0) close(segment_fd);
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