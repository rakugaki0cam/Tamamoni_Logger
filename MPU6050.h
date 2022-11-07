/*  MPU6050.h
 * 
 *      Accelerometer
 *      Gyroscope
 *
 *      6-axis sensor     
 * 
 *  2021.01.20 
 * モーション処理を分離
 * 
*/

#ifndef MPU6050_H
#define MPU6050_H

//センサーの軸と向きの設定(ICの取付方向による)
#define     WORLD_X             2   //X軸:sensorz  
#define     WORLD_Y             1   //Y軸:sensory
#define     WORLD_Z             0   //Z軸:sensorx
#define     DIRECTION_AX        -1
#define     DIRECTION_AY        1
#define     DIRECTION_AZ        1
#define     DIRECTION_GX        -1
#define     DIRECTION_GY        -1
#define     DIRECTION_GZ        -1


//グローバル変数
extern int16_t     rawdata_accel[3][RINGBUF_MAX + 1];   //Gデータのストア　　　　　@motion.c
extern int16_t     rawdata_gyro[3][RINGBUF_MAX + 1];    //ジャイロデータのストア　　@motion.c
extern uint16_t    motion_log_remaindata[];             //センサーFIFOデータ残数　16bit 0～1024バイト
extern uint16_t    sample_period;          //MPU6050 サンプリング間隔 usec   @measure.c　@motion.c
extern int16_t     accel_offset[];         //Gオフセット値                  @motion.c
extern int16_t     accel_conv[];           //Gゲイン                       @motion.c
extern uint16_t    accel_DLPF_delay;       //G DigitalLowPassFilterのディレイ  @motion.c
extern int16_t     gyro_offset[];          //ジャイロオフセット値              @motion.c
extern int16_t     gyro_conv[];            //ジャイロゲイン                   @motion.c
extern uint16_t    gyro_DLPF_delay;        //ジャイロ　DLPFのディレイ          @motion.c
extern uint8_t     ringbuf_count;          //加速度データリングバッファカウント @measure.c　@motion.c


bool        MPU6050_initialize(void);
void        MPU6050_interrupt(void);
void        MPU6050_read_rawdata(void);
uint16_t    MPU6050_read_fifo(void);
uint16_t    MPU6050_read_fifo_count(void);
bool        MPU6050_read_fifo_overflow(void);
bool        MPU6050_check_fifo_remain(void);
void        MPU6050_fifo_reset(void);
void        MPU6050_print_temp(void);

// test
void        wait_for(uint8_t);
void        MPU6050_SelfTest_accel(void);
void        MPU6050_SelfTest_gyro(void);
void        MPU6050_offset_accel(void);
void        MPU6050_offset_gyro(void);

#endif //MPU6050_H