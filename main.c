/*
 * Bullet Logger V9
 *      -- motion graph -- 
 * 
 *      v0, ve, impact time  measurement
 *      RL tilt & FR elevation angle            MPU6050
 *      temperature, humidity and pressure      BME280
 *      calender time                           RX8900
 * 
 *      LC115H case
 *      color LCD 2.4" 240x320
 *      Li-ION Battery
 *      Touch panel 
 *      microSD card
 *      RS485 LAN(1,2)
 *      WiFi ESP32-WROOM-32E ESO-NOW
 *
 * 
 *      Vdd = 3.3V 
 * 
 * 
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC18F57Q43
        Driver Version    :  2.00
 * 
 * �s���_           unicode �傫��
 *                  FATFS��FIL�̒�`��header.h�ւ��Ȃ��Ƃł��Ȃ�?
 
 * 
 * 2020.08.29   ver.0.00    BulletLogger3����ڐA
 *                          PIC18F26K83 -> PIC18F27Q43
 * 2020.08.29   ver.0.01    OK! DMA1���ז����Ă���LCD���^�����������B
 * 2020.08.29   ver.0.02    SD�J�[�hFATfs�I�b�P�[�B�v���O�����������[50% -> 73% 30k�o�C�g�قǎg����
 * 2020.08.30   ver.0.03    XC8�œK��-O2(�t���[�ł�2�܂ł͉�)�@SD��CSV�f�[�^�����o��
 * 2020.08.31   ver.0.04    �^�b�`�p�l�������t���O���������������� 
 * 2020.08.31   ver.0.05    SD�J�[�hcsv�f�[�^�Ƀw�b�_�s��ǉ�
 * 2020.09.02   ver.0.06    �v���O�����̌������A�܂Ƃ�
 * 2020.09.05   ver.0.07    ���[�V�����f�[�^�ʃt�@�C���֏o��
 * 2020.09.08   ver.0.08    LCD SPI DMA�]���@
 * 2020.09.12   ver.1.00    �Ƃ肠�����̊�����
 * 2020.09.13   ver.1.01    �X���[�v(�V���b�g�_�E��)�@�\�ǉ�
 * 2020.09.14   ver.1.02    ���荞�݂ƕ��ʂ̗�������ĂԃT�u���[�`����2�̃v���O���������������
 *                          (POWER OFF�T�u����sprintf���Ă�ł�������16k�o�C�g����������Q��Ă���)
 * 2020.09.15   ver.1.03    �o�b�e���[�d���ቺ�x�� 2���X�Ίp�x�v�Z����
 * 2020.09.16   ver.1.04    ���[�V�����Z���T�@�Q�C���␳ 3���X�Όv�Z
 * 2020.09.27   ver.1.05    �Œ蕶�����const�ɂăv���O�����������ֈړ��@RAM��2.5k���󂢂�???
 * 2021.01.03   ver.1.07    �w�b�_�t�@�C�����ł̒�`��������ړ������C��
 * 2021.01.11   ver.1.08    �t�@�C������2���ɓ��t�����ꂽ�@�Z���T�[3-4������e���v�Z�l���C��
 * 2021.01.12   ver.1.09    ���蒆�C���W�P�[�^�ǉ��@
 * 2021.01.13   ver.1.10    ���[�o�b�e���[���o�e�X�g�@���܂����A���[�V�����u���b�N�\������
 * 2021.01.16   ver.1.11    ���[�V�����u���b�NSD�J�[�h�o�̓o�O�t�B�N�X
 * 2021.01.16   ver.1.12    �V���b�g���̊p�x��������ƕ�?�^�C�~���O������
 * 
 * _V5 edition
 * 2021.01.20   ver.2.01    BulletLogger4����ڐA�@MPU6050�Z���T���ƃ��[�V���������ɕ�����DMP�����O����
 * 2021.01.22   ver.2.02    ���[�V�����O���t�\���@�u���b�N�\���A�W���C���\���p�~
 * 2021.01.25   ver.2.03    �^�C�}�[���荞��: Vmeasure��STOP���Ă��犄��������ƃ_���B�����������Ă��܂��^�C���A�E�g�ɂȂ�
 * 2021.01.30   ver.2.04    SD�J�[�h�G���[�\���ʒu�𑼂̃E�H�[�j���O�Ɠ���
 * 2021.01.30   ver.2.05    SD SPI MODE3->MODE0�ύX�B(���Ȃ�)
 * 
 * _V6 edition
 * 2021.05.02   ver.6.01    BOXED_BulletLogger5����ڐA�B�@PIC18F57Q43 48pin TQFP
 * 2021.05.04   ver.6.02    MPU6050�̒����l�����킹��(�r��...)�B�o�b�e���[�d���A�[�d���̕\��(�v���A�b�v�ǉ�)
 * 2021.05.05   ver.6.03    ���t���p�x�␳�l�����B�B�B�B�{�c�@�@�X��LED�ɔ��f����Ȃ��B
 * 2021.05.06   ver.6.04    ���t���p�x�␳�l������MPU6050.c��accel_offset�ōs���B
 *                          �}�E���g������Ԃ�MPU6050.c #define TEST_YES�ɂăI�t�Z�b�g�l�𑪒肵�Z�b�g
 * 2021.05.10   ver.6.05    �o�b�N�u�[�X�g�d�����n�C�p���[���[�h��
 * 2021.05.11   ver.6.06    RTCC���SOSC�֍����x32.768kHz���́BTMR3�Ńe�X�g->VE_CONTROL1(LAN[2])�֏o��Video_Sync ��LED��_��
 * 2021.05.12   ver.6.07    �o�b�e���d��AD�R���o�[�^Vref->FVR2.048V(+-4%)����1/3�ɕύX
 * 2021.05.22   ver.6.08    �s�b�`-0.5�x��0�ɏC�� -> �p�x�ɂ��ȈՕ␳��ǉ� @MPU6050.c
 * 2021.05.23   ver.6.09    �o�b�e����d���p���[�I�t(���~���R���o�[�^�Ȃ̂�HLVD�͂��܂������Ȃ�)
 * 2021.07.11               �r�f�I�V���N��1��ړ_������������^�C�~���O���x��Ă���i���荞�݁j->PWM�n�[�h�ŏ������Ă�
 *                          �^�C�}�[1�ł̃J�E���g��SMT1�ł�������萳�m�i�ȒP�j�ɂȂ邩��
 * 2021.07.24   ver.6.10    �X��LED 0.45�� �� 0.20��
 * 2021.08.14   ver.6.11    ���[�V�����^�C�~���OTMR3->TMR5�֕ύX(TMR3�p�~�̍ۂɖY��Ă����o�O�t�B�N�X)
 * 
 * _V7 edition
 * 2021.08.20   ver.7.01    �ʃZ���T�v����SMT�ōs���B�@->�L���v�`�����[�h�@�Z���T�M��->CLC1�ō���
 * 2021.09.17               ���[�V�����f�[�^��20msec�قǒx��Ă���???
 * 2021.09.20   ver.7.02    SMT����OK�B�܂�IOC�ƕ��p�Ńf�o�O�B�^�C�}�[�����������瓮���Ȃ��Ȃ����B
 * 2021.09.21   ver.7.03    SMT�L���v�`���[�l����prise_edge[],fall_edge[]�̒�`�ɂ���ē����Ȃ��Ȃ�B
 *                          �Y�����̑傫���A�錾�̈ʒu�H�H�@�܂�fileopenerror���ł�SD�������݂ł��Ȃ��B
 *                          ->30�ȏ��OK�݂����B
 * 2021.09.21   ver.7.04    �r�f�I�V���NLED TMR3->PWM3_16BIT (6Hz-duty7%)�ɕύX�B
 *                          TMR1,3,5��p�~
 * 2021.09.23   ver.7.05    vmeasure����A���S���Y���ύX�BPT1~4_IOC�͎c���B
 * 2021.09.25   ver.7.06    TIMING_LOG��ǉ����Ċ����̃^�C�~���O������f�o�b�O�BSD�֏o�́B
 * 2021.09.27   ver.7.07    LCD�\���֘A�������������ɕύX(�X�^�[�g��������������A�c�����������ƃ��[�V�����f�[�^�̎擾�������o�邽��)
 * 2021.09.28   ver.7.08    �������̑����BI2C�N���b�N 100kHz -> 400kHz (MFINTOSC 500kHz -> HFINTOSC 2MHz)
 * 2021.10.02   ver.7.10    ���[�V�����Z���T�[�f�[�^��FIFO�œǂݏo���B�f�[�^�������Ȃ��Ȃ����B
 *                          ���܂�FIFO�I�[�o�[�t���[���o��????????????
 * 2021.10.05   ver.7.11    �O���t�̃V���b�g�ԓ_�̈ʒu�𐳊m��
 * 2021.10.06   ver.7.12    �X�Ίp�x�\�����σT���v�������ł��邾������(�����O�o�b�t�@�� �� �\�������Ԃ̃f�[�^���ɕύX)
 * 2021.10.09   ver.7.13    �u���v�Z�̌^�w���s�K���̂��ߐ����Ɋۂ܂��Ă����Ƃ��낪�������̂ŏC���B
 *                          ���܂ɌX�Ίp�x�̒l���X�V����Ȃ����Ƃ�����B�B�B�B�s��
 * 2021.10.10   ver.7.14    �O���t�\����motion_graph.c�ɕ������B���\���͈͂��L�����BFIFO�Ǎ����C���B(�܂��s������)
 * 2021.10.11   ver.7.15    �Ƃ肠�����A���[�V�����f�[�^�Ō�܂œǂݍ��񂾌�FIFO�f�[�^���c���Ă��邩���`�F�b�N���ĉ�ʂ֌x���\��
 * 2021.10.13   ver.7.16    PT1~4�Z���T�[�V�[�P���X�ُ�̎��^�C���A�E�g�BPT1�ȊO���ŏ��ɓ��͂������͖�������B
 * 2021.10.17   ver.7.17    i2c���[�`�������Ɛ��ɁB1MHz x 1/4 = 250kHz�܂ł��������Ȃ��B�@�@(�܂���2MHz x 1/5 = 400kHz��OK)
 * 2021.10.21   ver.7.18    ���[�V�����^�C�~���O���؃r�f�I240fps�ɓ���(PWM3_16BIT)
 * 2021.12.19   ver.7.19    RTC_disp()����hour���X�V�\������Ȃ��o�O�t�B�N�X
 * 2022.01.30   ver.7.20    �{�^�����������Ƃ���SD�J�[�h�`�F�b�N�ł���悤��
 * 2022.01.30   ver.7.21    SD�J�[�h�`�F�b�N�̓��C�����[�v�ōs���B��2�b���B
 * 2022.02.01   ver.7.30    �������胂�[�h����肽��*********************************
 * 
 * _V8 edition
 * 2022.04.19   ver.8.01    LAN�R�l�N�^�s���A�T�C���ύX�BPT1,2,3,4�̃v���A�b�v��100k�v���_�E���ɕύX���A�ڑ��m�F���ł���悤�ɂ����B
 * 2022.04.23   ver.8.02    RS485�Ή��@UART1->2�@�s���ύX�@TX2,RX2,TXDE(Tx Driver Enable)
 *                          VE_CONTROL1(Video-Sync LED) VE_CONTROL2
 *                          MCC�����藐���MCCmelody��system�Ƃ����t�H���_��t�@�C�����ז������Ă����B�폜����OK
 * 2022.04.24   ver.8.03    ���[�V�����\����؂藣���B
 * 2022.04.24               ��M�e�X�g
 * 
 * _V8_2
 * 2022.04.25   ver.8.04    MCC��WIN10�ŏC���BSM1T�֘A�啝�ύX�B    IOC,CCP1,TMR1,CLC1,2,3,7,8
 * 2022.04.29   ver.8.05    I2C�s��->250kHz��OK�B(OSCFRQ=1:2MHz->=0:1MHz,I2CCON2 FMC1/4) 
 *                          CCP1,2�L���v�`���[�������Ȃ�???_>CLC���͂��_���݂���
 * 2022.04.30   ver.8.06    TMRxG�ɂđ���BTMR1-CLC5-PT12��TMR3-CLC6-PT34
 *                          �Z���T1-2����=�@�B���@
 * 2022.05.02   ver.8.07    MCC core.5.0.2->5.4.2��WIN10�ŕύX�BSDCard.c�͑O�̂���R�s�[
 *                          ����ł�WIN�ŊJ����3���ȏォ����BMAC�ł͊J���Ȃ��͗l�B
 *                          sd_spi.c 940�s�@READ_MULTI_BLOCK�`�@�o�O�t�B�N�X�@CRC��4�o�C�g������
 * 2022.05.03   ver.8.08    ���蒆�������[�v�΍�@TMR0OVF�Ō��o3sec
 *                          �Z���T3-4����=�@�B���@
 * 2022.05.04   ver.8.09    �f�[�^��Mscanf��OK�B���[�V�����s��
 * 2022.05.05               MCC core5.4.3�@SDcard�������̂ňȑO�̃o�[�W�����̂��̂��g�p�BMCC����͐ݒ�ł��Ȃ�(���ƂŃ������������̂ŕs�m��)
 * 2022.05.08   ver.8.10    �ڑ��@�탂�[�h�ʂɕ\����ʂ𐮗��B���[�V�����O���t�ƃ^�[�Q�b�g�̕\���؂�ւ��B(���[�h�ɂ��)
 * 2022.05.12   ver.8.11    �o�O�t�B�N�X
 * 2022.05.14   ver.8.12    �o�O�t�B�N�X�B�A���O���\�����_��->motion_clear()�̔z�u���_����motion_gate�����܂������Ă��Ȃ������B
 *              ver.8.13    ���[�h��ς������ɂ��^�[�Q�b�g�\���ɒ��e�_���ĕ`��(����50�����܂�)
 * 2022.05.15   ver.8.14    ���e=�ԐF�A�O�̐Ղ��`���F�ɂ��Č��ݒ��e�����₷�������B
 * 2022.05.18               �^�[�Q�b�g���Ƀr�f�I�V���N��LED���t��
 * 2022.05.21   ver.8.15    v0���[�h�ɏ����ꗗ�\��&���ϒl�v�Z
 * 2022.05.22   ver.8.16    �W�e�Z���^to�Z���^�̌v�Z�\��       
 * 2022.06.01   ver.8.17    �o�O�t�B�N�X�[CtoC�\����V0ave�\�����_�u��BV0_MODE�̎�CtoC�N���A���Ȃ��B
 * 2022.08.06   ver.8.18    �����v�ڑ������̎��̌X�Ίp�f�[�^�̓��O���Ȃ��B(�^�C�~���O�����Ȃ�����)
 * 2022.08.07   ver.8.19    ���[�V�������O�̃^�C���I�t�Z�b�g���[���̃o�O�t�B�N�X(lag_time�v�Z�l��sec������ -> x1000000����usec��)
 * 2022.08.08   ver.8.20    �����v�ڑ��Ń��[�V�����\�������������̃��[�h��ǉ��B���鎞��LAN���ʃR�l�N�^���g�p�B
 * 2022.08.15   ver.8.21    CtoC�̃T���v������\��
 * 2022.08.16   ver.8.22    CtoC�����Z�b�g(�^�[�Q�b�g�����Z�b�g)
 *                          �����[�[�[�[�[�[�[
 * 2022.08.20   ver.8.23    v0�`���[�g�̕\���s�X�N���[���̃o�O�t�B�N�X
 * 2022.10.15   ver.8.30    SW2,SW3�ǉ��@MCC core v5.4.11
 * 2022.10.16   ver.8.31    SW2-�^�[�Q�b�g�f�[�^���Z�b�g�ASW3-�����Z���T1-2�����؂�ւ�
 * 2022.10.16   ver.8.32    �X�g���{�̏����ƍ���Ȃ�1~2usec ??????�����s���@�� �X�g���{���u���̃~�X(�Z���T�G�b�W���ʂ̂��Ȃ��Ȃ鑤rise�ɊԈႦ�Ă���)
 * 2022.10.17   ver.8.33    SW2-v0�`���[�g�f�[�^�����Z�b�g
 * 2022.10.17   ver.8.34    �p���[�I���������̃^�C�~���O��ύX(SYSTEM_INIT�̑O��__delay�͂�������)�^�C�g����ʒǉ�
 * 
 * _V8_3
 * 2022.10.19   ver.8.50    �z�b�v��]������X�g���{�p���O���ڂ�ǉ�
 * 2022.10.20   ver.8.51    ���C�����j���[��ʂ𐧍�J�n�B
 *                          tmp_string[140]��SD�J�[�h�ɏ����o�����s�����Ă����͗l�B[256]�ɑ��ʁB
 * 2022.10.22   ver.8.52    TOUCH_INT��RB4->RC5�֕ύX(LCD�������ݎ��Ƀm�C�Y������)�B�֌W�Ȃ������B
 *                          TOUCH_INT��������������ƃ^�b�`���Ă����ON�ɂȂ���ςȂ��Ȃ̂ŕω�����������Ȃ��B���荞�ݖ����ɕύX
 *                          EEP ROM�����l��������
 *                          �o�b�N���C�g���A
 * 2022.10.23   ver.8.55    ���C�����j���[�^�b�`��ʂ̌��`�����B�������΍�B�J���[���[�h18bit�̂݁BdotRGB[960]->[580]�֌��炵�ăf�[�^���������m��
 * 2022.10.23   ver.8.56    FATfs��LongFileName����߂ă������[��2.6k�������B�V���[�g��8.3�����B�t�@�C�����ύX�BdotRGB[1280]�ɁB
 * 2022.10.27   ver.8.57    �^�b�`8�񕽋ρ[�[�[BUGBUG��
 * 2022.11.01   ver.8.58    SD�J�[�h�֕ۑ��̃t�@�C�����̊g���q��.txt����.csv�ɕύX�B�@�^�b�`XPT2046�̉��x�W����0.3141�ɕύX->�_��
 * 2022.11.01   ver.8.59    �ۑ��t�@�C���l�[���̐ڔ���"A"��"Z"����(EEPROM���g��) �w�b�_���������̂Ƃ��͍X�V���Ȃ��B
 * 2022.11.05   ver.8.60    �t�b�^���X�V�T�u�B���j���[��ʍ쐬���B
 * 2022.11.05   ver.8.61    �Z�b�g�A�b�v���j���[�Ƃ肠�����ł���
 * 2022.11.06   ver.8.62    �����̐��l���͂�m��mm�ɂ킯��B���j���[�{�^���z�u�ύX�B
 * 2022.11.08   ver.8.63    �^�[�Q�b�g�̌v�Z�G���[��Rx�G���[�𕪂��ĕ\���B
 * 2022.11.09   ver.8.64    v0�`���[�g�ɃW���[���\���ǉ��B
 * 2022.11.10   ver.8.65    �֌W�̂Ȃ���MUART�f�[�^����������B->TARGET�Ή�
 * 
 * _V9 edition
 * 2023.01.28   ver.9.00    ESP32WiFi��ǉ��ɔ����A�n�[�h�ύX
 *                          �s��45 PT4_WiFi�@CLCin
 *                          �s��39��45����ύX�@LAN7 VIDEO_LED�@PWM31��PWM11
 *                          RS485 UART2->UART4�ɕύX
 *                          SPI2�p�~����SPI1�ɓ����@�@3�s������
 *                          ESP32�ʐM�@UART2
 *                          MCC���@����export��1���ۑ��B�_���Ȃ��͓̂Ǎ�fail�ɂȂ�
 *                                  MC3�t�@�C����V�����ꏊ�Ɏ����čs���āAMCC�ŊJ��generate����Ƃ��ꂢ�ȃt�@�C�����ł���->user���̏C�����K�v
 *                                  mcc_generaateed_files�t�H���_���Ƃ�������R�s�[���Ă�git�������č������킩��̂ŕύX�C���͊y
 * 2023.02.05   ver.9.01    ESP����̃^�[�Q�b�g�f�o�C�X�`�F�b�N�Ǝ�M�̐ݒ�
 * 2023.02.07   ver.9.02    target_mode�ϐ������[�J�����B(target_mode_get()�֐���ǉ�)
 * 2023.02.20   ver.9.03    ESP32�Ƃ̃f�[�^�����UART2��9600bps����115200bps�ɕύX
 * 2023.02.25   ver.9.04    UART�̐����BCtoC�f�[�^��2�����̂Ƃ��\������
 * 2023.02.26   ver.9.05    1���ڂ̓��͕s�ǂ̒����B�V���A��DEBUGger�֎�M�f�[�^�\���BLAN�ڑ�����ƃV���A���\���̓L�����Z�������B�[�[�[�[�[�[�[
 * 2023.03.11   ver.9.06    BB��6mm�\���ɁBUART�o�b�t�@�N���A�������ύX�BDEBUGger�\���BLAN�̎��͕\�����Ȃ��B
 * 2023.03.12   ver.9.07    �v���O��������������̃��j��-�f�[�^���s���ɂȂ邱�Ƃ�����Beeprom�ǂݏo���̎��Ƀ`�F�b�N�B
 * 2023.03.18   ver.9.10    �^�[�Q�b�g�ݒ胁�j���[��ǉ�
 * 2023.03.22   ver.9.11    �^�[�Q�b�g���j���[�ɃL�����Z���ǉ��B���^�[�����̓Z�b�g���Ă���->�Z�b�g&���^�[���{�^���ɕύX
 * 2023.03.25   ver.9.12    �^�[�Q�b�g�L���ڑ�����printf����
 * 2023.04.15   ver.9.13    �^�[�Q�b�gCtoC�W�vn=49���܂Ł@-> 80���@�G���[�\���B  memory Data 89->91%
 * 2023.04.22   ver.9.14    �^�[�Q�b�g�N���A���CtoC���O�l���O��̒l�ɂȂ��Ă���̂��C��
 * 2023.04.23   ver.9.15    �^�[�Q�b�g�ݒ�l��EEPROM�֕ۑ����A�N�������f�B���O�ɑ_�_������ǉ�
 * 
 * 
 *  ////���[�V�����Z���T�[�̔��ˎ��̃^�C�~���O���O�擾��SMT1�̃^�C�}�[���g���Ă��邯��ǁA���e���Ă��܂��ƃ^�C�}�[���~�܂��Ă��܂��\��������B�ʃ^�C�}�[�ɂ��邩???
 * 
 * 
 * 
 * 
 * �ۗ������A�s�����
 * �n�[�h���Z�b�g��������Ȃ�?
 * �f�o�b�O�̎��ASD�J�[�h�Ŏ~�܂�BSD�����Α��v�Ȃ悤???
 * ���[�V�������荞�݁@IOC->INT0~2�ɂ������BIOC���荞�݂̓s���̔��ʂɈ��Ԃ������Ă���̂ŁA���Ԃ�������B
 * RTCC���荞�݁@IOC->INT0~2�ɂ�����
 * VE�I�����[���[�h��VE�Z���T�ŏ�������B���쎩�̂�V0���[�h�Ɠ����ꗗ�\���Ƃ�����
 * �f�o�b�O�̎��AI2C�ʐM���ɒ�~����ƃo�X�G���[�ɂȂ�݂����B�d���؂�Ȃ��ƕ��A���Ȃ��B���Z�b�g�������Ȃ��B
 * �ʔ��˂̎��̃��[�V�����̌v�Z�B�O�f�[�^�Ƃ̔䗦�ʒu�ŋ��߂�B�g���K�[�������O�̐Î~��Ԃ̐��l�ł̓_���݂����Ȃ̂ŁB
 * ���O�f�[�^��WiFi�ƗL���̋L�ڂ�ǉ�������
 *  WiFi�y�A�����O����Ă��鎞�ɁA�d�q�^�[�Q�b�g����ESP32�Ƀv���O�������������ނƃ^�}���j���t���[�Y����?????
 * �^�[�Q�b�g�f�o�C�X�ɕω������������ɒ��ʃf�[�^���c�邱�Ƃ�����B���ʃf�[�^�̃N���A�̃^�C�~���O?
 * 
 * command_uart_send() target_set_up_command() ���̕Ӑ�������target_graph.c��
 * 
 * 
 * 
 * 
*/
#include    "header.h"

__EEPROM_DATA (0x00, 0x23, 0x01, 0x01, 0x41, 0x00, 0x07, 0xf4); //eeprom_address_t
//              v0    Y     M     D     suf   out   m     mmL
__EEPROM_DATA (0x01, 0x02, 0xfa, 0x00, 0x07, 0xc3, 0x00, 0xf1); 
//              mmU   gun   bbL   bbU   BB    nuL   nuU   osY
__EEPROM_DATA (0x4a, 0x5a, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff); 
//              aiY   brL   brU
//Picket�̐ݒ��preserve�Ƀ`�F�b�N����ƃv���O�����������ݎ��ɏ����������Ȃ�


#define     BACKLIGHT_OFF_TIME  (5 * 60)    //5min                          
#define     SLEEP_TIME          (9 * 60)    //9min ���̌�J�E���g�_�E��60sec


//global
const char  title[] = "Bullet Logger V9";
const char  version[] = "9.15"; 
char        tmp_string[256];    //sprintf������p
uint8_t     dotRGB[1280];        //�ςł��Ȃ� 2�{�p����576�o�C�g
bool        sw1_int_flag = 0;   //SW1�����t���O

//FATfs�֘A �����Œ�`���Ȃ��ƃX�y�[�X�m�ۂł����ɃG���[
FATFS       drive;
FIL         file;
UINT        actualLength;



#ifdef  TIMING_LOG   //debug
uint8_t     time_log;
uint8_t     time_point[66];
uint24_t    time_stamp[66];
uint8_t     time_remain[66];
#endif  //TIMING_LOG

//local
uint8_t     part_num;           //���C�����[�v���̏������������Ԕԍ�
bool        RTC_error;          //I2C�Z���T�[�G���[�`�F�b�N
bool        Atmos_error;
bool        Motion_error;
bool        bloff_flag;         //�o�b�N���C�g�I�t�̃t���O
bool        sleep_flag;         //�X���[�v�̃t���O
bool        low_battery_int_flag;
uint16_t    touch[3];           //�^�b�`���W
uint8_t     i;

/*
                         Main application
 */

//debug timing log
#ifdef TIMING_LOG                   //debug
void DEBUG_timing_log(uint8_t point){
    //�^�C�~���O����@�f�o�b�O�p
    if (measure_status == VMEASURE_READY) {
        return;
    }
    time_point[time_log] = point;
    time_stamp[time_log] = SMT1TMR;
    time_remain[time_log] = motion_message_remain;
    time_log++;
    if(time_log > 63){
        time_log = 63;
    }
}
#endif


void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    DMA1_StopTransfer();            //DMA1_Initialize�œ���J�n����Ă���̂ŃX�g�b�v������@�@DMA1 -> SPI1�ɐݒ肵�Ă���
    i2c_master_initialize();        //�萻�V���v��I2C�h���C�o
    //�o�b�N�u�[�X�g�d��

    __delay_ms(500);                //��Ⓑ�����Ƃ���
    if(SW1_PORT == SW_ON){
        POWER_MODE_SetLow();        //L:�n�C�p���[���[�h, H:�p���[�Z�[�u
        POWER_EN2_SetHigh();        //�o�b�N�u�[�X�g�d��3.3V�I��
    }
    
    //******* �p���[�I�� *************
    LED_RIGHT_SetHigh();     
    LED_LEFT_SetHigh();                          //�X��LED�𗼕��Ƃ��_��
    
    //RTCC���W���[���N���b�N�o�̓I��
    RTCC_FOE_SetHigh();
    
    IOCAF4_SetInterruptHandler(detect_SW1); //�X�C�b�`1�����R�[���o�b�N�֐����蓖��
    vmeasure_initialize();
    
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    
    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();
    
    //�����t���O�N���A
    sw1_int_flag = 0;
    RTC_int_flag = 0;                     
    low_battery_int_flag = 0;
    //low_battery_count = 0;

    LCD_initialize();
    PWM3_LoadDutyValue(100 * (0x03ff / 100));   //LCD�o�b�N���C�g�I���@100% (10bit)

    //LCD �^�C�g��
    LED_RIGHT_SetLow();     
    LED_LEFT_SetLow();                          //�X��LED�𗼕��Ƃ�����
    LCD_Title();
    __delay_ms(200);
    
    printf("\n");
    printf("********************\n");
    printf(" Bullet Logger V9   \n");
    printf("           ver.%s\n", version);
    printf(" AUTO Device mode   \n");/////////ver8.10-//////////////////////////
    printf(" WiFi ESP32         \n");/////////ver9.00-//////////////////////////
    printf("           Feb 2023 \n");
    printf("********************\n");
    
    touch_init();
    
    //I2C
    RTC_error = RTC_initialize();
    Atmos_error = BME280_initialize();
    Motion_error = MPU6050_initialize();
    motion_initialize();
    angle_level_tilt_disp_init();
   
    BatV_disp(1);                               //�t���\��
    BME280_disp(1);                             //�t���\��
    RTC_disp(1);                                //�S�̕\���@�N���b�N�ɓd���f���������Ƃ��̓Z�b�g�^�C����ʂɂȂ�B
    
    generate_filename_date();                   //�ۑ��p�t�@�C���l�[������
    if (shotheader_sd_write() == 0){
        //�w�b�_�[���������߂��̂�SD�J�[�h�͓����Ă���
        ShotHeader_write_flag = 1;              //�w�b�_�����o���ς�
    }else{
        ShotHeader_write_flag = 0;
    }
    
    while(SW1_PORT == SW_ON){
        //�X�C�b�`�I���̃L�[�𗣂��̂�҂�
    }
    sw1_int_flag = 0;
    
    //EEP ROM�ǂݍ���
    read_rom_main();
    read_rom_setup();
 
    //�^�C�g���\����
    for(i = 0; i < 200; i++){
        __delay_ms(10);
        footer_rewrite(1);      //���v���̕\���X�V�̂�
        motion_data_read();
        angle_level_disp();     //�X�Ίp�x��LED
    }
    LCD_Title_Clear();              //�^�C�g���I��
    
    sensor_connect_check();         //�}�g���@��̐ڑ��`�F�b�N�[���胂�[�h�̕\���؂�ւ�
    //target_lcd_default_command();   //�d�q�^�[�Q�b�g�Ƀf�t�H���g�Z�b�g�R�}���h�𑗂�
    //target_set_up_command();
    vmeasure_ready();
    
    //***** MAIN LOOP **********************************************************
    //
    
    while (1){
        //�e������ƃ��[�V�����\��
        measure_status = vmeasure();
        footer_rewrite(0);
        if(measure_status == VMEASURE_READY){
            input_mmi();
        }
        
    }
}


void footer_rewrite(bool setup_flag){
    //RTC1�b�X�V�����Ŏ����C�ۃo�b�e�����̕\�����X�V
    //�e�T�u���[�`���͏����������B����J�n�������́A�T�u�֔�΂Ȃ����Ă���B
    //setup_flag:�Z�b�g�A�b�v���j���[����̌Ăяo�����B���v�A�V�C�A�o�b�e���̕\���̂�
    if ((RTC_int_flag == 1) && (measure_status == VMEASURE_READY)){//////////////RTC_int_flag==1�𐮗��ł�����//////////
        RTC_disp(0);    //�Ⴄ�Ƃ������\��
    }
    if ((RTC_int_flag == 1) && (measure_status == VMEASURE_READY)){
        BME280_disp(0); //���l�����������\��
    }
    if ((RTC_int_flag == 1) && (measure_status == VMEASURE_READY)){
        BatV_disp(0);   //���l�����������\��
    }
    if ((RTC_int_flag == 1) && (measure_status == VMEASURE_READY) && (setup_flag == 0)){
        sensor_connect_check();     //�}�g���@��̐ڑ��`�F�b�N�[���胂�[�h�̕\���؂�ւ�
    }
    if ((RTC_int_flag == 1) && (measure_status == VMEASURE_READY) && (setup_flag == 0)){
        switch (part_num){
            case 0:
                low_battery();
                part_num++;
                break;
            case 1:
                if (ShotHeader_write_flag == 0){ 
                    //SD�J�[�h�������Ă��Ȃ����͊m�F����
                    //�������̂̊m�F�͂��Ȃ�-->���ׂ��傫��
                    generate_filename_date();                   //�ۑ��p�t�@�C���l�[������
                    if (shotheader_sd_write() == 0){
                        //�w�b�_�[���������߂��̂�SD�J�[�h�͓����Ă���
                        ShotHeader_write_flag = 1;              //�w�b�_�����o���ς�
                    }
                }
                part_num++;
                break;
#if 0
            case 2:
                MPU6050_disp_temp();
                part_num++;
                break;
#endif
            default:
                part_num = 0;
                break;

        }
    }
    RTC_int_flag = 0;                    //�t���O�����Z�b�g
  
}


void input_mmi(void){
    //Man Machine Interface
    //���C�����[�v���̃L�[�������&�d���֘A
    //���쒆�͌����Ƃ͂Ȃ��͂��Ȃ̂ŁA����J�n���̃f�B���C�΍�͖���
    //����T�C�N���X�^�[�g���Ă��Ȃ����̂ݏ���
    uint8_t     c = 0;
    uint8_t     tmp;                //UART�̂ēǂ�

    if (TOUCH_INT_PORT == 0){
        //�^�b�`�p�l�����͂���
        touch_xyz(touch);
        sleep_count_reset();
    }
       
    if (sw1_int_flag == 1){
        //SW1�������ꂽ
        LED_RIGHT_SetLow();     
        LED_LEFT_SetLow();          //�X��LED�𗼕��Ƃ�����
        sleep_count_reset();
        power_switch();             //����񉟂�:�Z�b�g�A�b�v�A������:�d���I�t
        sw1_int_flag = 0;
        setup_menu();
        sleep_count_reset();
        return;
    }

    if (SW2_PORT == SW_ON){
        //SW2�������ꂽ
        sleep_count_reset();
        target_data_reset();    //�^�[�Q�b�g�f�[�^�����Z�b�g
        v0_data_reset();        //v0�`���[�g�f�[�^�̃��Z�b�g
    }

    if (SW3_PORT == SW_ON){
        //SW3�������ꂽ
        sleep_count_reset();
        set_v0sensor(1);    //1:change
    }
    
    //rx_buffer_clear();      //UART�o�b�t�@�̃S�~���̂ēǂ� LAN�̂ق��͂�������イ�N���A����Ă��܂�???/////////////////////////////
    
    if (sleep_count >= BACKLIGHT_OFF_TIME){
        if (bloff_flag == 0){
            //�o�b�N���C�g�I�t����
            PWM3_LoadDutyValue(15 * (0x03ff / 100));           //LCD�o�b�N���C�g�I�t�@15% (10bit) 
            bloff_flag = 1;
        }
    }
    
    if (sleep_count >= SLEEP_TIME){
        shut_down();
    }

}


void detect_SW1(void){
    //�����{�^�����荞��
    sw1_int_flag = 1;
}


void Stop_until_SW1(void){
    //SW1���������܂ŃX�g�b�v�����Ă���
    sprintf(tmp_string, "Push button to next ");
    LCD_Printf(COL_WARNING, ROW_WARNING1, tmp_string, 1, PINK, 1);
    sw1_int_flag = 0;
    
    do {
    //sw1���������܂Ń��[�v
    } while((sw1_int_flag == 0) && (TOUCH_INT_PORT == 1));
    
    sprintf(tmp_string, "                    ");
    LCD_Printf(COL_WARNING, ROW_WARNING1, tmp_string, 1, BLACK, 1); 
    sw1_int_flag = 0;
    
    while(SW1_PORT == SW_ON){
        //�X�C�b�`�I���̃L�[�𗣂��̂�҂�
    }
    __delay_ms(500);
    
}


uint8_t BatV_disp(bool full_disp){
    //�o�b�e���[�d���𑪒�
    //full_disp �t���\��
    
#define     ADFVR                   //AD�R���o�[�^���t�@�����X�d��+ VDD / FVR
#ifndef     ADFVR
    #define     VREF    3.263       //VDD = LTC3558 3.3V�o�b�N�u�[�X�g�d���@�����l
    #define     R1      4680        //������R+�� ��
    #define     R2      4680        //������R-��
#else
    #define     VREF    (2.048 * 1.00988)   //ADFVR = FVR1:x2(+-4%)  �����l�ŕ␳
    #define     R1      4680                //������R+�� ��
    #define     R2      2348                //������R-��
#endif
#define     LOW_BATTERY_VOLTAGE     3.20    //��d������
    
    static uint8_t      disp_stage = 0;
    static uint16_t     batv;
    static float        voltage;
    
    do {
        switch (disp_stage){
            case 0:
            case 1:
                //�d����AD�ϊ�
                DIVIDER_ON_SetHigh();               //������R���I��
                __delay_us(2);
                ADCC_StartConversion(BAT_V_AN);
                while(!ADCC_IsConversionDone());
                batv = ADCC_GetConversionResult();
                DIVIDER_ON_SetLow();                //������R���I�t
                disp_stage = 2;
                break;
            case 2:
                //�����v�Z
                voltage = (float)(batv * (((float)R1 + R2) / R2) * VREF / 4096);
                disp_stage = 3;
                break;
            case 3:
                //�d������
                if (voltage <= LOW_BATTERY_VOLTAGE){
                    low_battery_int_flag = 1;               
                    ///HLVD�ł̓}�C�R����3.3V�𔻒肷�邯��ǁA���~���R���o�[�^�̂��ߒ�d���ɂȂ�ɂ���
                }else{
                    low_battery_int_flag = 0;               
                }
                disp_stage = 4;
                break;
            case 4:
                //�t���[����ʕ\��
                if (full_disp){
                    sprintf(tmp_string, "BAT %4.2fV", voltage);
                    LCD_Printf(COL_BATV, ROW_BATV, tmp_string, 1, YELLOW, 1);
                }
                disp_stage = 5;
                break;
            case 5:
                //�d����ʕ\��
                sprintf(tmp_string, "%4.2f", voltage);
                LCD_Printf((COL_BATV + 6 * 4), ROW_BATV, tmp_string, 1, YELLOW, 1); //4�����E����
                disp_stage = 6;
                break;
            case 6:
                //�[�d���̕\��
                if (POWER_CHRG_PORT == 0){
                    LCD_Printf((COL_BATV + 6 * 10), ROW_BATV, "CHRG", 1, RED, 1); //10�����E����
                }else{
                    LCD_Printf((COL_BATV + 6 * 10), ROW_BATV, "    ", 1, BLACK, 1);
                }
                disp_stage = 0;
                break;

        }
    } while((full_disp == 1) & (disp_stage != 0)); 
        
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(18);       //BatV return
#endif
        return disp_stage;
}


void sleep_count_reset(void){
    //�X���[�v�J�E���g�����Z�b�g����
    //�o�b�N���C�g�������͕��A
    sleep_count = 0;
    if (bloff_flag == 1){
        PWM3_LoadDutyValue(100 * (0x03ff / 100));           //LCD�o�b�N���C�g�I�t�@100% (10bit) 
        bloff_flag = 0;
    } 
}


void    power_switch(void){
    //���j���[�X�C�b�`�������`�p���[�I�t
    //SW1���荞�݃t���O���o���炭��
    uint8_t i;
    
    //2�b������
    for (i = 0; i < 20; i++){
        __delay_ms(100);
        if (SW1_PORT == SW_OFF){
            sw1_int_flag = 0;                           //����񉟂��̎��̓t���O���Z�b�g���ă��^�[��
            return;
        }
    }
    write_rom_setup();///////setup���炭��Ƃ��ɕK�v
    power_off();
}

void    power_off(void){
    //�d���I�t
    LED_RIGHT_SetHigh();     
    LED_LEFT_SetHigh();                                 //�X��LED�𗼕��Ƃ��_��
    LCD_SetColor(0,0,0);                                //��
    LCD_DrawFillBox(10, 120, 229, 160);                 //�h��Ԃ��l�p
    sprintf(tmp_string, "POWER OFF!!");
    LCD_Printf( 20, 130, tmp_string, 3, PINK, 0);       //3�{�p��DMA�`�ʕs��
    __delay_ms(1000);
    
    //LCD clear
    PWM3_LoadDutyValue(0 * (0x03ff / 100));             //LCD�o�b�N���C�g�I�t�@0% (10bit) 
    LCD_ClearScreen(0);                                 // ��ʂ�0=���A1=���ŏ�������
    LED_RIGHT_SetLow();     
    LED_LEFT_SetLow();                                  //�X��LED�𗼕��Ƃ�����
    //power off
    POWER_EN2_SetLow();
    while(1);
}


void    shut_down(void){
    //LCD��������̓d���I�t
    uint16_t     i = 600;                               //100msec x 600 = 60sec�J�E���g�_�E��
    
    LED_RIGHT_SetLow();     
    LED_LEFT_SetLow();         //�X��LED�𗼕��Ƃ�����
    sleep_flag = 1;
    LCD_SetColor(0,0,0);                                //��
    LCD_DrawFillBox(10, 128, 229, 207);                 //�h��Ԃ��l�p
    sprintf(tmp_string, "SHUT DOWN!!");
    LCD_Printf( 20, 136, tmp_string, 3, PINK, 0);       //3�{�p��DMA�ł̕`�ʕs��

    do {
        //�J�E���g�_�E��
        sprintf(tmp_string, "%2d sec   ", i / 10);
        LCD_Printf( 70, 180, tmp_string, 2, YELLOW, 1);
        RTC_disp(0);    //�Ⴄ�Ƃ������\��
        BME280_disp(0); //���l�����������\��
        BatV_disp(0);   //���l�����������\��
        __delay_ms(80);                                 //�����Œ���
        if ((sw1_int_flag == 1) || (TOUCH_INT_PORT == 0)){
           //SW1,�^�b�`�Œ��~
           sleep_flag = 0;
           sw1_int_flag = 0;
           break;
        }
        i--;
    }while (i > 6);

    if (sleep_flag == 1){
        LCD_ClearScreen(0);                             // ��ʂ�0=���ŏ���
        //power off
        POWER_EN2_SetLow();
        while(1);
    }
    //�V���b�g�_�E�����~
    LCD_SetColor(0,0,0);                                //��
    LCD_DrawFillBox(0, 120, 239, 264);                  //�h��Ԃ��l�p
    sleep_count_reset();
    print_targetmode(INDIGO);                          //��ʍĕ`��
    vmeasure_ready();
} 


void    low_battery(void){
    //�o�b�e���[��d��
#define     LOW_BAT_TIME    10      //x1sec(rtc_int�Ō��Z)
    
    enum {
        IDLE,
        SET,
        COUNTDOWN,
        BOUNCE_BACK,
        POWER_OFF
    };
    static uint8_t  low_bat_stat = 0;
    
    switch(low_bat_stat){
        case IDLE:
            if (low_battery_int_flag == 1){                
                low_bat_stat = SET;
                low_battery_int_flag = 0;
            }
            break;
            
        case SET:
            low_battery_count = LOW_BAT_TIME;   //�J�E���^���Z�b�g
            low_bat_stat = COUNTDOWN;
            break;
            
        case COUNTDOWN:
            if (low_battery_int_flag == 0){
                low_bat_stat = BOUNCE_BACK;
            }
            
            sprintf(tmp_string, "Low Battery%2d", low_battery_count);
            LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, PINK, 1);
            low_battery_count--;      //rtc_int()��1sec���ƂɌ��Z
            if (low_battery_count <= 0){
                low_bat_stat = POWER_OFF;
            }
            break;
            
        case BOUNCE_BACK:
            sprintf(tmp_string, "              ");
            LCD_Printf(COL_WARNING, ROW_WARNING1 , tmp_string, 1, BLACK, 1);
            low_bat_stat = IDLE;
            print_targetmode(INDIGO);                      //��ʍĕ`��
            break;
            
        case POWER_OFF:
            LCD_SetColor(0,0,0);                            //��
            LCD_DrawFillBox(10, 128, 229, 207);             //�h��Ԃ��l�p
            sprintf(tmp_string, "LOW BATTERY!");
            LCD_Printf( 20, 156, tmp_string, 3, PINK, 0);   //3�{�p��DMA�ł̕`�ʕs��
            __delay_ms(3000);
            //LCD clear
            PWM3_LoadDutyValue(0 * (0x03ff / 100));         //LCD�o�b�N���C�g�I�t�@0% (10bit) 
            LCD_ClearScreen(0);                             // ��ʂ�0=���A1=���ŏ�������
            //power off
            POWER_EN2_SetLow();
            while(1);
            break;
    }
#ifdef  TIMING_LOG                  //debug
        DEBUG_timing_log(19);       //Low Bat return
#endif
}


void    read_rom_main(void){
    //EEP ROM����f�[�^����
    sensor_type = DATAEE_ReadByte(V0SENS12_ADDRESS);    //�����v�̎��
}



/**
 End of File
*/
