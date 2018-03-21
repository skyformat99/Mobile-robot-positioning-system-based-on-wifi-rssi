#ifndef __KALMAN_H__
#define __KALMAN_H__

// һά�˲�����Ϣ�ṹ��
typedef struct{
	double filterValue;  //k-1ʱ�̵��˲�ֵ������k-1ʱ�̵�ֵ
	double kalmanGain;   //   Kalamn����
	double A;   // x(n)=A*x(n-1)+u(n),u(n)~N(0,Q)
	double H;   // z(n)=H*x(n)+w(n),w(n)~N(0,R)
	double Q;   //Ԥ���������ƫ��ķ���
	double R;   //��������ƫ�(ϵͳ����Ժ�ͨ������ͳ��ʵ����)
	double P;   //�������Э����
}  KalmanInfo;

void Init_KalmanInfo(KalmanInfo* info, double Q, double R);
double m_KalmanFilter(KalmanInfo* kalmanInfo, double lastMeasurement);
double KalmanFilter(const double ResrcData,
                    double ProcessNiose_Q,double MeasureNoise_R);
#endif
