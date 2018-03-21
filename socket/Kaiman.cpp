//#include <iostream>
//#include <Windows.h>
#include "Kalman.h"

/**
* @brief Init_KalmanInfo   ��ʼ���˲����ĳ�ʼֵ
* @param info  �˲���ָ��
* @param Q Ԥ���������� ��ϵͳ�ⲿ�ⶨ����
* @param R ������������ ��ϵͳ�ⲿ�ⶨ����
*/
void Init_KalmanInfo(KalmanInfo* info, double Q, double R)
{
	info->A = 1;  //����������
	info->H = 1;  //
	info->P = 1;  //����״̬����ֵ���ķ���ĳ�ʼֵ����ҪΪ0���ⲻ��
	info->Q = Q;    //Ԥ�⣨���̣��������� Ӱ���������ʣ����Ը���ʵ���������
	info->R = R;    //�������۲⣩�������� ����ͨ��ʵ���ֶλ��
	info->kalmanGain = 0;
	info->filterValue = 0;// �����ĳ�ʼֵ
    //OutputDebugStringA("��ʼ����ϣ�\n");
	//std::cout << info.filterValue << std::endl;
	//while (true);
}

double m_KalmanFilter(KalmanInfo* kalmanInfo, double lastMeasurement)
{
	//Ԥ����һʱ�̵�ֵ
	double predictValue = kalmanInfo->A* kalmanInfo->filterValue;   //x�������������һ��ʱ���ĺ������ֵ��������Ϣ�������˴���Ҫ���ݻ�վ�߶���һ���޸�

	//��Э����
	kalmanInfo->P = kalmanInfo->A*kalmanInfo->A*kalmanInfo->P + kalmanInfo->Q;  //������������� p(n|n-1)=A^2*p(n-1|n-1)+q
	double preValue = kalmanInfo->filterValue;  //��¼�ϴ�ʵ�������ֵ

	//����kalman����
	kalmanInfo->kalmanGain = kalmanInfo->P*kalmanInfo->H / (kalmanInfo->P*kalmanInfo->H*kalmanInfo->H + kalmanInfo->R);  //Kg(k)= P(k|k-1) H�� / (H P(k|k-1) H�� + R)
	//����������������˲�ֵ
	kalmanInfo->filterValue = predictValue + (lastMeasurement - predictValue)*kalmanInfo->kalmanGain;  //���ò������Ϣ���ƶ�x(t)�Ĺ��ƣ�����������ƣ����ֵҲ�������  X(k|k)= X(k|k-1)+Kg(k) (Z(k)-H X(k|k-1))
	//���º������
	kalmanInfo->P = (1 - kalmanInfo->kalmanGain*kalmanInfo->H)*kalmanInfo->P;//������������  P[n|n]=(1-K[n]*H)*P[n|n-1]

	return  kalmanInfo->filterValue;
}

double KalmanFilter(const double ResrcData,
                    double ProcessNiose_Q,double MeasureNoise_R)
{
        double R = MeasureNoise_R;
        double Q = ProcessNiose_Q;
        static        double x_last;
        double x_mid = x_last;
        double x_now;
        static        double p_last;
        double p_mid ;
        double p_now;
        double kg;
        x_mid=x_last; //x_last=x(k-1|k-1),x_mid=x(k|k-1)
        p_mid=p_last+Q; //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
        kg=p_mid/(p_mid+R); //kgΪkalman filter��RΪ����
        x_now=x_mid+kg*(ResrcData-x_mid);//���Ƴ�������ֵ
        p_now=(1-kg)*p_mid;//����ֵ��Ӧ��covariance
        p_last = p_now; //����covarianceֵ
        x_last = x_now; //����ϵͳ״ֵ̬
        return x_now;
}
