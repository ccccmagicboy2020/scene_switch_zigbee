/*

�򵥵Ķ��������ϵͳ

��ʵֻ�и���������л�,��˵����OS�е�ǣǿ,��������һЩ�򵥵Ŀ���Ӧ����˵,��Ҳ��������õ�.�������չ����.�����˰���ĳɹ����������.

����һ����򵥵�OS,һ��������Ч��Ϊ��,������,�л�һ�������20����������,Ҳ�����ڱ�׼51(������12M����)��20uS.
��Ϊ�ٶ�������������,Ϊ�˸�ÿ�����񶼷���һ��˽�ж�ջ,��ռ���˽϶���ڴ�.��Ϊ����,����������װ��ų����߼�,�Ӷ����Խ�ʡһЩ���ڿ��Ƶı���.
�����Խ��,ռ���ڴ�Խ��,������ҲԽ�ð���,��ʵ�������������������Ŀ.һ����˵,4�����㹻.���ҿ�����һ���۳�����Ϊ���,���뻻��һЩ��ʱ����.

task_load(������,����ۺ�)
װ������

os_start(����ۺ�)
���������.��������ָ��һ��װ���˵�����,����ϵͳ�����.

task_switch()
�л�����������



.��д������ע������:
KEIL C�������Ǽٶ��û�ʹ�õ����񻷾�,�����ڱ�����ʹ���϶�δ�Զ�������д���,��д����ʱӦע��������Ǻʹ�����������.

1.����:������Ϊ�˽�ʡ�ڴ�,�������û�õ��ù�ϵ�ĺ�������ͬһ�ڴ��ַ��Ϊ�����ռ�.���ڵ��������Ǻܺ�����,�����ڶ�������˵,�������̻ụ����ŶԷ�.
����ķ�����:���������ڻ��Խtask_switch()�ı���,��ʹ��staticǰ�,��֤���ַ�ռ����ʱ��Ψһ��.

2.����:���벢���Ƕ������¶��е�����,�ڵ�����ʱ,�����ݹ�ͬ���ᵼ������,��,һ�������Ĳ�ͬʵ��(���߽���"����")֮��ı�����������.
����ķ�����:ʹ��reentrant�������(����:void function1() reentrant{...}).��Ȼ,�����İ취���Ǳ�������,��Ϊ���������޴��Ŀ�������,�����󽵵�����Ч��.

3.��������һ��,�ڱ�����,����������Ϊһ����ѭ��.�˳������ᵼ��ϵͳ����.




.������������û��д�ɻ���Ƕ���,�л�����ʱӦ��ע��ʲô����?

����KEIL C�������ڴ�����������ʱ��Լ������Ϊ"�Ӻ����п����޸�����Ĵ���",��˱������ڵ���ǰ���ͷ����мĴ���,�Ӻ������迼�Ǳ����κμĴ���.
�����д�߻�������˵�е㲻ϰ��: ���ϰ�������ӳ����б����Ĵ���.
��ע��һ��ԭ��:������Ҫ��Խtask_switch()�ļĴ���,ȫ����Ҫ����(������ջ).��������취����,��Ҫ�üĴ�����Խ�����л�����task_switch()
��ʵ������Ҫ����һ��,����ǰ��˵,���ڱ��������ڱ�����ַ�����Ż�,��˷��ǷǾ�̬���������ÿ�Խtask_switch().


����������д:
void ������(void){//���������붨��Ϊ�޲�����
while(1){//���������÷���,����Ϊ��ѭ��
	//....����д����������
	
	task_switch();//ÿִ��һ��ʱ������,���ͷ�CPUһ��,�ñ�������л�������.
}
}


����װ��:
task_load(������,����ۺ�)

װ�غ����Ķ����ɷ���������ʱ��,��ͨ������main()��.Ҫע�����,�ڱ���������û�������񻻳�,
������ִ��os_start()ǰ���뽫���������װ��.֮������������������е�����.



�������������:
os_start(����ۺ�)

���øú��,���Ӳ���ָ��������ۿ�ʼִ���������.����Ϊÿ�л�һ����������⿪��20����������,����Ǩ�ƶ�ջ.
*/












#include <reg51.h>


/*============================����Ϊ�������������============================*/

#define MAX_TASKS 3//����۸���.�ڱ����в�δ���������뻻��,����ʵ�����е������ж��ٸ�,�Ͷ�����ٸ������,���ɶඨ����ٶ���

//�����ջָ��
unsigned char idata task_sp[MAX_TASKS];
										
#define MAX_TASK_DEP 12 //���ջ��.��Ͳ�������2��,����ֵΪ12.
						//Ԥ������:��2Ϊ����,ÿ����һ�㺯������,��2�ֽ�.��������ܷ����ж�,��Ҫ�ټ����ж���Ҫ��ջ��.
						//��Сջ��ķ���:1.������Ƕ���ӳ��� 2.���ӳ���ǰ���ж�.
unsigned char idata task_stack[MAX_TASKS][MAX_TASK_DEP];//�����ջ.

unsigned char task_id;//��ǰ������


//�����л�����(���������)
void task_switch(){
	task_sp[task_id] = SP;

	if(++task_id == MAX_TASKS)
		task_id = 0;

	SP = task_sp[task_id];
}

//����װ�뺯��.��ָ���ĺ���(����1)װ��ָ��(����2)���������.����ò���ԭ����������,��ԭ����ʧ,��ϵͳ�������ᷢ������.
void task_load(unsigned int fn, unsigned char tid){
	task_sp[tid] = task_stack[tid] + 1;
	task_stack[tid][0] = (unsigned int)fn & 0xff;
	task_stack[tid][1] = (unsigned int)fn >> 8;
}

//��ָ��������ʼ�����������.���øú��,����������.
#define os_start(tid) {task_id = tid,SP = task_sp[tid];return;}




















/*============================����Ϊ���Դ���============================*/


unsigned char stra[3], strb[3];//�����ڴ�鸴�Ʋ��Ե�����.




//��������:�����ڴ��.ÿ����һ���ֽ��ͷ�CPUһ��
void task1(){
	//ÿ����һ���ֽ��ͷ�CPUһ��,����ѭ���ı������뿼�Ǹ���
	static unsigned char i;//������������ǰ��staticȥ��,�ᷢ��ʲô��?
	i = 0;

	while(1){//�������Ϊ��ѭ��,�����˳�����,����ϵͳ�����
		stra[i] = strb[i];
		if(++i == sizeof(stra))
			i = 0;

		//����i�������Խ��task_switch(),��������붨��Ϊ��̬(static),���������ᱻ���������޸�,��Ϊ����һ��������Ҳ���õ��ñ�����ռ�õĵ�ַ.
		task_switch();//�ͷ�CPUһ���,�����������л�������.���ȥ������,���Ľ�����Զ���ᱻ���õ�
	}
}

//��������:�����ڴ��.ÿ����һ���ֽ��ͷ�CPUһ��.
void task2(){
	//ÿ����һ���ֽ��ͷ�CPUһ��,����ѭ���ı������뿼�Ǹ���
	static unsigned char i;//������������ǰ��staticȥ��,���ᷢ����������.task1()��task2()�ᱻ���������䵽ͬһ���ڴ��ַ��,����������ͬʱ����ʱ,i��ֵ�ͻᱻ�������������ȥ
	i = 0;

	while(1){//�������Ϊ��ѭ��,�����˳�����,����ϵͳ�����
		stra[i] = strb[i];
		if(++i == sizeof(stra))
			i = 0;

		//����i�������Խ��task_switch(),��������붨��Ϊ��̬(static),���������ᱻ���������޸�,��Ϊ����һ��������Ҳ���õ��ñ�����ռ�õĵ�ַ.
		task_switch();//�ͷ�CPUһ���,�����������л�������.���ȥ������,���Ľ�����Զ���ᱻ���õ�
	}
}

//��������:�����ڴ��.�����������ֽں��ͷ�CPUһ��.
void task3(){
	//����ȫ���ֽں���ͷ�CPU,����ѭ���ı������뿼�Ǹ���
	unsigned char i;//�������ǰ����Ҫ��static,��Ϊ�������������ڲ�û���ͷŹ�CPU

	while(1){//�������Ϊ��ѭ��,�����˳�����,����ϵͳ�����
		i = sizeof(stra);
		do{
			stra[i-1] = strb[i-1];
		}while(--i);

		//����i���������������ʹ��,�������趨��Ϊ��̬.���������Զ���Ϊ�Ĵ�����(regiter)
		task_switch();//�ͷ�CPUһ���,�����������л�������.���ȥ������,���Ľ�����Զ���ᱻ���õ�
	}
}

void main(){
	//�����ʾ���ﲢû�п�������Ļ��뻻��,��������۱���ȫ������,����ϵͳ�����.
	//����װ������������,����ڶ���MAX_TASKSʱҲ���붨��Ϊ3
	task_load(task1, 0);//��task1����װ��0�Ų�
	task_load(task2, 1);//��task2����װ��1�Ų�
	task_load(task3, 2);//��task3����װ��2�Ų�

	os_start(0);//�����������,����0�Ųۿ�ʼ����.������Ϊ1,����������1�Ų�.
				//���øú��,�������������ٷ���main(),Ҳ����˵,�������֮���������䶼����ִ�е�.
}
