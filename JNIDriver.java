package com.wz.jnidriver;

public class JNIDriver {
    private boolean isConnected;
    final int SUCCESS = 1;
    final int FAIL = -1;
    
    static {
        System.loadLibrary("JNICDriver");
    }
    
    private native static int openDrivers();
    private native static void closeDrivers();
    
    public JNIDriver {
        isConnected = false;
    }
    
    public int open() {
        if (isConnected) {
            return FAIL;
        }
        
        if (openDrivers() < 0) {
            return FAIL;
        }
        
        return SUCCESS;
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
}