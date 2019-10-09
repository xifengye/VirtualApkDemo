/* DO NOT EDIT THIS FILE - it is machine generated */

#include <string.h>
#include <jni.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>


#include <android/log.h>

#define  LOG_TAG    "canjni_native"
#define  LOGI(...)  __android_log_print (ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#include <linux/can/raw.h>
#include "libsocketcan.h"
#include "com_can_Canjni.h"

//

//==============add by zl for 2.4 G(RF24L01 )===================================================
int rf_fd=NULL;
int RX_TX_Flag=NULL;
#define  TxWIDTH   8
#define  RxWIDTH   8

JNIEXPORT jint JNICALL Java_com_can_Canjni_OpenRf24l01(JNIEnv *env, jclass thiz ) {

    int ret=0;
    if (rf_fd == 0) {

        //      LOGD("RF24L01 send");
        rf_fd = open("/dev/nRF24l01", O_RDWR);//读写方式
    }
    else  LOGE(" open  device first!\n");
    return ret;
}


JNIEXPORT jint JNICALL Java_com_can_Canjni_CloseRf24l01(JNIEnv *env, jclass thiz ) {

  close(rf_fd);
    rf_fd=NULL;
    return 0;
}


JNIEXPORT jint JNICALL Java_com_can_Canjni_SendRfData(JNIEnv *env, jobject thiz,jbyteArray Txbuf ,jint length) {

    int ret=0;
    //==============取数据，JAVA中以对像的形式传下数据，要通过其获取元素数据===================================
    if (rf_fd == 0) {
//        LOGE("please open rf24l01 device first!\n");
        //      LOGD("RF24L01 send");
        rf_fd = open("/dev/nRF24l01", O_RDWR);//读写方式
    }
    jbyte *buf = (jbyte *) malloc(length+8);
    if (!buf) {
//        LOGE("java/lang/OutOfMemoryError\n");
        return -1;
    }
     RX_TX_Flag=1;
    (*env)->GetByteArrayRegion(env, Txbuf, 0, length, buf);

    ret = write(rf_fd, buf, length);

    free(buf);

    if (ret < 0) {
 //       LOGE("failed to send RF device,ret=%d\n", ret);
        RX_TX_Flag=0;
        return -1;
    } else {
        //  LOGI("read RF device ok,data size=%d\n", ret);
        RX_TX_Flag=0;
        return ret;
    }

}

JNIEXPORT jint JNICALL Java_com_can_Canjni_ReceiveRfData(JNIEnv *env, jclass thiz ,jbyteArray Rxbuf ) {
    int ret=0;
//================================================
    if( RX_TX_Flag)
         return 0;
    if (rf_fd == 0) {
//        LOGE("please open can device first!\n");
  //      LOGD("RF24L01 send");
        rf_fd = open("/dev/nRF24l01", O_RDWR);//读写方式
    }
    jbyte *buf = (jbyte *) malloc(RxWIDTH+8);
    if (!buf) {
//        LOGE("java/lang/OutOfMemoryError\n");
        return -1;
    }

      ret = read(rf_fd, buf, RxWIDTH);
    if (ret == 0) {
        (*env)->SetByteArrayRegion(env, Rxbuf, 0,TxWIDTH, buf);
        ret=RxWIDTH;
    }
    free(buf);

    if (ret < 0) {
//        LOGE("failed to read RF device,ret=%d\n", ret);
        return -1;
    } else {
      //  LOGI("read RF device ok,data size=%d\n", ret);
        return ret;
    }

}

//=============rf24l01 add end by zl ========================================================================
/*
 * Class:     com_example_canjni_Canjni
 * Method:    open_can
 * Signature: ()I
 */
static int gFD = 0;

JNIEXPORT jint JNICALL Java_com_can_Canjni_open_1can(JNIEnv *env, jclass thiz,
                                                                       jint ctrl_mode, jint bitrate) {
//    LOGI("start open can device\n");

    //0.stop can
    if (can_do_stop("can0") < 0) {
//        LOGE("failed to stop can0 \n");
        return -1;
    } else {
//        LOGI("can0 bus is stoped");
    }


    //1.set bitrate
    if (can_set_bitrate("can0", bitrate) < 0) {
//        LOGE("failed to set bitrate \n");
        return -1;
    } else {
//        LOGI("set can0 bitrate to %d\n", bitrate);
    }

    //2.set ctrl mode
    struct can_ctrlmode cm;
    memset(&cm, 0, sizeof(cm));
    cm.flags |= ctrl_mode;
    cm.mask |= ~ctrl_mode;

    if (can_set_ctrlmode("can0", &cm) < 0) {
//        LOGE("failed to set ctrlmode\n");
        return -1;
    } else {
//        LOGI("set can0 ctrl mode to %d\n", ctrl_mode);
    }

    //3.start can

    if (can_do_start("can0") < 0) {
//        LOGE("failed to start can0 \n");
        return -1;
    } else {
//        LOGI("can0 bus is start\n");
    }


    int s = -1;
    struct sockaddr_can addr;
    struct ifreq ifr;
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
//        LOGE("socket error\n");
        return -1;
    }

    strncpy(ifr.ifr_name, "can0", sizeof(ifr.ifr_name));
    if (ioctl(s, SIOCGIFINDEX, &ifr)) {
//        LOGE("ioctl get index error\n");
        return -1;
    }
    addr.can_ifindex = ifr.ifr_ifindex;
    addr.can_family = PF_CAN;

    if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
//        LOGE("bind socket error\n");
        return -1;
    }

    gFD = s;
    return 0;
}

/*
 * Class:     com_example_canjni_Canjni
 * Method:    close_can
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_can_Canjni_close_1can(JNIEnv *env, jclass thiz) {
//    LOGI("start close can device\n");

    int err = 0;
    err = close(gFD);
    return err;
}

/*
 * Class:     com_example_canjni_Canjni
 * Method:    read_can
 * Signature: ([BI)I
 */
JNIEXPORT jint JNICALL Java_com_can_Canjni_read_1can(JNIEnv *env, jclass thiz,
                                                                       jbyteArray buffer, jint length) {
//    LOGI("start read can device\n");

    if (gFD == 0) {
//        LOGE("please open can device first!\n");
        return -1;
    }

    jbyte *buf = (jbyte *) malloc(length);
    if (!buf) {
//        LOGE("java/lang/OutOfMemoryError\n");
        return -1;
    }

    int ret = read(gFD, buf, length);
    if (ret > 0) {
//        LOGI("can_id=0x%x,can_dlc=%d,data[]=0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n",
//             ((struct can_frame *) buf)->can_id, \
//
//             ((struct can_frame *) buf)->can_dlc, \
//
//             ((struct can_frame *) buf)->data[0], \
//
//             ((struct can_frame *) buf)->data[1], \
//
//             ((struct can_frame *) buf)->data[2], \
//
//             ((struct can_frame *) buf)->data[3], \
//
//             ((struct can_frame *) buf)->data[4], \
//
//             ((struct can_frame *) buf)->data[5], \
//
//             ((struct can_frame *) buf)->data[6], \
//
//             ((struct can_frame *) buf)->data[7]);
        (*env)->SetByteArrayRegion(env, buffer, 0, ret, buf);
    }

    free(buf);

    if (ret < 0) {
//        LOGE("failed to read can device,ret=%d\n", ret);
        return -1;
    } else {
//        LOGI("read can device ok,data size=%d\n", ret);
        return ret;
    }
}

#define CAN_FRAME_STANDARD              0x01
#define CAN_FRAME_EXTEND                  0x02
#define CAN_FRAME_DATA                          0x04
#define CAN_FRAME_REMOTE                  0x08
/*
 * Class:     com_example_canjni_Canjni
 * Method:    write_can
 * Signature: ([BI)I
 */
JNIEXPORT jint JNICALL Java_com_can_Canjni_write_1can(JNIEnv *env, jclass thiz,
                                                                        jbyteArray buffer, jint length,
                                                                        jint frame_type) {
//    LOGI("start write can device\n");
    jbyte *buf = (jbyte *) malloc(length);
    struct can_frame frame;
    int i = 0;

    if (gFD == 0) {
//        LOGE("please open can device first!\n");
        return -1;
    }

    if (!buf) {
//        LOGE("java/lang/OutOfMemoryError\n");
        return -1;
    }
    (*env)->GetByteArrayRegion(env, buffer, 0, length, buf);

    memset(&frame, 0, sizeof(frame));

    //calculate can id in frame struct
    frame.can_id = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
    if (frame_type & CAN_FRAME_EXTEND) {
        frame.can_id &= CAN_EFF_MASK;
        frame.can_id |= CAN_EFF_FLAG;
    } else {
        frame.can_id &= CAN_SFF_MASK;
    }
    if (frame_type & CAN_FRAME_REMOTE)
        frame.can_id |= CAN_RTR_FLAG;

    //calculate can dlc in frame struct,can_dlc means size of data to send,must <=8
    frame.can_dlc = buf[4];
    if (frame.can_dlc > 8)
        frame.can_dlc = 8;

    //calculate can data in frame struct
    for (i = 0; i < frame.can_dlc; i++)
        frame.data[i] = buf[5 + i];
//    LOGI("can_id=0x%x,can_dlc=%d,data[]=0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n", frame.can_id, \
//
//         frame.can_dlc, \
//
//         frame.data[0], \
//
//         frame.data[1], \
//
//         frame.data[2], \
//
//         frame.data[3], \
//
//         frame.data[4], \
//
//         frame.data[5], \
//
//         frame.data[6], \
//
//         frame.data[7]);

    //write frame to can device
    jint ret = write(gFD, &frame, sizeof(frame));
    free(buf);

    if (ret < 0) {
//        LOGE("failed to write can device,ret=%d\n", ret);
        return -1;
    } else {
//        LOGI("write can device ok,data size=%d\n", ret);
        return ret;
    }
}

