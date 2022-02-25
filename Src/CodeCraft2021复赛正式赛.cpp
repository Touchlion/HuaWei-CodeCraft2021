#include<iostream>
#include<cmath>
#include<algorithm>
#include<iomanip>
#include<cstring>
#include<map>
#include<vector>
using namespace std;
typedef long long LL;
int inf = 1e9;
int N;
int M;
int T;
int K;
int R;
int total_sev = 0;
int total_vma = 0;
int total_sev1 = 0;
int total_vma1 = 0;
int total_idle_sev = 0;
int CPU_core_demand = 0;
int instorage_demand = 0;
double x1 = 1.75; double x2 = 2.201; double x3 = 3.28; double x5 = 0.3;//������Ϊ�����Ĳ���
double x4 = 1.6;//������Ϊ�����Ĳ������з��յĲ�����Ӧ����һ��1
double idle_rate(){ return 1.0*total_idle_sev / total_sev; }//�����������ʡ�(0,1)
void strcpy1(char model[], char *&m)
{
	int len = strlen(m);
	for (int i = 0; i < len; i++)
	{
		model[i] = m[i];
	}
	model[len] = '\0';
}
void strcpy2(char model[], char m[])
{
	int len = strlen(m);
	for (int i = 0; i < len; i++)
	{
		model[i] = m[i];
	}
	model[len] = '\0';
}
int strcmp1(char opt[], char s[])
{
	int len = strlen(s);
	for (int i = 0; i < len; i++)
	{
		if (opt[i] != s[i])return 0;
	}
	return 1;
}

struct Sever //�������ͺţ�������100�֣�
{
	char model[24];//�ͺ���
	int core; //CPU����
	int memory; //�ڴ�
	LL hardcost; //����ɱ�
	LL dailycost; //ÿ�ճɱ�
	void update(char *m = NULL, int c = 0, int me = 0, LL hc = 0, LL dc = 0)
	{
		if (m)strcpy1(model, m);
		core = c; memory = me; hardcost = hc; dailycost = dc;
	}
	double evaluate(int left_day)//�ۺϿ��ǣ�������ͺŵļ�/�Ա�
	{
		return (hardcost + dailycost*left_day)*T*1.0 / (core + memory - abs(memory - core) / x1);
	}
}sever[105];

struct Vmachine //������ͺ�
{
	char model[24];
	int core;
	int memory;
	int isbinode; //0��ʾ���ڵ㲿��1��ʾ˫�ڵ�
	void update(char *m = NULL, int c = 0, int me = 0, int isb = 0)
	{
		if (m)strcpy1(model, m);
		core = c; memory = me; isbinode = isb;
	}
	void copy(Vmachine a)
	{
		if (a.model)strcpy2(model, a.model);
		core = a.core; memory = a.memory; isbinode = a.isbinode;
	}
}vmachine[1005];
struct Vma //һ̨�����
{
	int type; //���ͼ����ԣ���vmachine�е��±꣩����0��ʼ����M-1������M<=1000���ͺŵ��������
	int vma_id; //����id
};

struct Node //�������е�һ�����
{
	int core_all = 0; int memory_all = 0;
	int core_used = 0; int memory_used = 0;
	Vma vma[1025]; int number_vma = 0;
	int core_left(){ return core_all - core_used; }
	int memory_left(){ return memory_all - memory_used; }
};
struct Sev //һ̨������
{
	int type;//���ͼ����ԣ���sever�е��±꣩����0��ʼ����N-1������N<=100���ͺŷ�������
	Node node[2];//0==A,1==B
	int total_core_used(){ return node[0].core_used + node[1].core_used; }
	int total_memory_used(){ return node[0].memory_used + node[1].memory_used; }
	int total_core_left(){ return node[0].core_left() + node[1].core_left(); }
	int total_memory_left(){ return node[0].memory_left() + node[1].memory_left(); }
	int total_number_vma(){ return node[0].number_vma + node[1].number_vma; }
}sev[100001];//��������з���������

struct Opt_list //��¼ÿ��Ĳ��������2e5��
{
	int top = 0;
	int opt_type[400005];//1��ʾadd��2��ʾdel
	int vma_type[400005];
	int vma_id[400005];
}daily_opt;
struct Migration_list
{
	int W = 0;
	int vma_id[200005]; int sev_id[200005];
	int sev_node[200005];//0��ʾA�ڵ㣬1��ʾB�ڵ㣬2��ʾ˫�ڵ㣨�������
}daily_migration;
struct Add_list
{
	int p = 0;
	int sev_id[200005]; int sev_node[200005];
}add_list;
struct Buy_list //��¼��������ķ�����
{
	int Q = 0;
	int type_buy_number[101];//��ʾÿ���ͺţ���sever[]���±꣩�ķ����������̨
	void init()
	{
		Q = 0;
		memset(type_buy_number, 0, sizeof(type_buy_number));
	}
}buy_list;

struct vma_node
{
	int vma_id;
	int vma_type;
	int sev_node;
	vma_node*next;
};
struct Vma_record//֮ǰ��Ѳ���������һ��������С˳�����������ȽϺ�ʱ��������Ǩ�ƣ���������Ǩ�Ʋ���������
{
	vma_node*head = NULL;
	int vma_need(vma_node*s)
	{
		return vmachine[s->vma_type].core + vmachine[s->vma_type].memory;
	}
	void add(int id, int type, int Sev_node)
	{
		if (head == NULL)
		{
			head = new vma_node;
			head->vma_id = id;
			head->vma_type = type;
			head->sev_node = Sev_node;
			head->next = NULL;

		}
		else
		{
			vma_node*p = new vma_node;
			p->vma_id = id;
			p->vma_type = type;
			p->sev_node = Sev_node;
			int p_need = vma_need(p);
			if (vma_need(head) > p_need)
			{
				p->next = head;
				head = p;
			}
			else
			{
				vma_node*s = head;
				while (s->next != NULL&&vma_need(s->next) <= p_need)s = s->next;
				if (s->next == NULL)
				{
					s->next = p; p->next = NULL;
				}
				else
				{
					p->next = s->next; s->next = p;
				}
			}
		}
	}
	void del(int id)
	{
		vma_node*s = head;
		if (head->vma_id == id)
		{
			head = head->next; delete s; return;
		}
		while (s->next != NULL&&s->next->vma_id != id)s = s->next;
		if (s->next->next == NULL)
		{
			delete s->next; s->next = NULL;
		}
		else
		{
			vma_node*q = s->next; s->next = s->next->next; delete q; q = NULL;
		}
	}
}vma_record0;//�����0��ʾֻ�ѵ��ڵ㲿����������������˫�ڵ�Ĳ���

map <int, int >mp;//�������idӳ�䵽�����������+1��
map<int, int>mp2;//�ӷ��������+1ӳ�䵽�����������Ϊ�ķ��������+1���Ż���ʱ���ǲ��ù�mp2��

void del_vma(int vma_id)//���������id����ɾ��
{
	int sevid = mp[vma_id] - 1; //��ȡ����������ڵķ�������id
	mp[vma_id] = 0; total_vma--;
	for (int i = 0; i < sev[sevid].node[0].number_vma; i++)
	{
		if (sev[sevid].node[0].vma[i].vma_id == vma_id)
		{
			int vmatype = sev[sevid].node[0].vma[i].type;
			if (vmachine[vmatype].isbinode == 0)
			{
				sev[sevid].node[0].core_used -= vmachine[vmatype].core;
				sev[sevid].node[0].memory_used -= vmachine[vmatype].memory;
				if (sev[sevid].total_core_used() + sev[sevid].total_memory_used() == 0)
				{
					total_idle_sev++;
				}
			}
			else if (vmachine[vmatype].isbinode == 1)
			{
				sev[sevid].node[0].core_used -= (vmachine[vmatype].core / 2);
				sev[sevid].node[0].memory_used -= (vmachine[vmatype].memory / 2);
				sev[sevid].node[1].core_used -= (vmachine[vmatype].core / 2);
				sev[sevid].node[1].memory_used -= (vmachine[vmatype].memory / 2);
				if (sev[sevid].total_core_used() + sev[sevid].total_memory_used() == 0)
				{
					total_idle_sev++;
				}
			}
			for (int j = i; j + 1 < sev[sevid].node[0].number_vma; j++)
			{
				sev[sevid].node[0].vma[j].type = sev[sevid].node[0].vma[j + 1].type;
				sev[sevid].node[0].vma[j].vma_id = sev[sevid].node[0].vma[j + 1].vma_id;
			}
			sev[sevid].node[0].number_vma--;
			return;
		}
	}
	for (int i = 0; i < sev[sevid].node[1].number_vma; i++)
	{
		if (sev[sevid].node[1].vma[i].vma_id == vma_id)
		{
			int vmatype = sev[sevid].node[1].vma[i].type;
			sev[sevid].node[1].core_used -= vmachine[vmatype].core;
			sev[sevid].node[1].memory_used -= vmachine[vmatype].memory;
			if (sev[sevid].total_core_used() + sev[sevid].total_memory_used() == 0)
			{
				total_idle_sev++;
			}
			for (int j = i; j + 1 < sev[sevid].node[1].number_vma; j++)
			{
				sev[sevid].node[1].vma[j].type = sev[sevid].node[1].vma[j + 1].type;
				sev[sevid].node[1].vma[j].vma_id = sev[sevid].node[1].vma[j + 1].vma_id;
			}
			sev[sevid].node[1].number_vma--;
			return;
		}
	}
}

int add_vma(int vma_type, int vma_id)//���������������ʵķ������ϲ���¼������0��ʾװ�����ˣ����򣻷���1��ʾ�ɹ�����
{
	int flag = 0;
	int min_core_left = 1e8; int min_memory_left = 1e8;
	int min_core_left0 = 1e8; int min_memory_left0 = 1e8;
	int min_core_left1 = 1e8; int min_memory_left1 = 1e8;
	int all = 1e9;
	int min_left_sev_id; int min_left_sev_node;
	int left_core; int left_memory; int need_core; int need_memory;
	int left_core0; int left_memory0; int left_core1; int left_memory1;
	int delta_vma; int delta_sev;
	int delta_sev0; int delta_sev1;
	double idle_rate_now = idle_rate();
	double x = x2;//����
	if (vmachine[vma_type].isbinode == 0) //���ڵ㲿��
	{
		need_core = vmachine[vma_type].core;
		need_memory = vmachine[vma_type].memory;
		delta_vma = need_core - need_memory;
		for (int i = 0; i < total_sev; i++)
		{
			left_core = sev[i].node[0].core_left();
			left_memory = sev[i].node[0].memory_left();
			delta_sev = left_core - left_memory;
			if (left_core >= need_core&&left_memory >= need_memory) //������װ������̨������ķ�������ѡ��
			{
				if (left_core + left_memory + abs(delta_sev - delta_vma) / x < all) //ѡ��һ����ӽ����ص�
				{
					all = left_core + left_memory + abs(delta_sev - delta_vma) / x;
					min_left_sev_id = i; min_left_sev_node = 0;
					flag = 1;
				}
			}
			left_core = sev[i].node[1].core_left();
			left_memory = sev[i].node[1].memory_left();
			delta_sev = left_core - left_memory;
			if (left_core >= need_core&&left_memory >= need_memory)
			{
				if (left_core + left_memory + abs(delta_sev - delta_vma) / x < all)
				{
					all = left_core + left_memory + abs(delta_sev - delta_vma) / x;
					min_left_sev_id = i; min_left_sev_node = 1;
					flag = 1;
				}
			}
		}
		if (flag == 0)return 0;
		mp[vma_id] = min_left_sev_id + 1;
		if (sev[min_left_sev_id].total_core_used() + sev[min_left_sev_id].total_memory_used() == 0)total_idle_sev--;
		sev[min_left_sev_id].node[min_left_sev_node].core_used += need_core;
		sev[min_left_sev_id].node[min_left_sev_node].memory_used += need_memory;
		sev[min_left_sev_id].node[min_left_sev_node].vma[sev[min_left_sev_id].node[min_left_sev_node].number_vma].type = vma_type;
		sev[min_left_sev_id].node[min_left_sev_node].vma[sev[min_left_sev_id].node[min_left_sev_node].number_vma].vma_id = vma_id;
		sev[min_left_sev_id].node[min_left_sev_node].number_vma++;
		add_list.sev_id[add_list.p] = min_left_sev_id; add_list.sev_node[add_list.p] = min_left_sev_node; add_list.p++;
		total_vma++;
		return 1;
	}
	else if (vmachine[vma_type].isbinode == 1) //˫�ڵ㲿��
	{
		need_core = vmachine[vma_type].core / 2;
		need_memory = vmachine[vma_type].memory / 2;
		delta_vma = need_core - need_memory;
		for (int i = 0; i < total_sev; i++)
		{
			left_core0 = sev[i].node[0].core_left();
			left_core1 = sev[i].node[1].core_left();
			left_memory0 = sev[i].node[0].memory_left();
			left_memory1 = sev[i].node[1].memory_left();
			delta_sev0 = left_core0 - left_memory0;
			delta_sev1 = left_core1 - left_memory1;
			if (left_core0 >= need_core&&left_core1 >= need_core&&left_memory0 >= need_memory&&left_memory1 >= need_memory)
			{
				if (left_core0 + left_core1 + left_memory0 + left_memory1 + abs(delta_sev0 - delta_vma) / x + abs(delta_sev1 - delta_vma) / x < all)
				{
					all = left_core0 + left_core1 + left_memory0 + left_memory1 + abs(delta_sev0 - delta_vma) / x + abs(delta_sev1 - delta_vma) / x;
					min_left_sev_id = i;
					flag = 1;
				}
			}
		}
		if (flag == 0)return 0;
		mp[vma_id] = min_left_sev_id + 1;
		if (sev[min_left_sev_id].total_core_used() + sev[min_left_sev_id].total_memory_used() == 0)total_idle_sev--;
		sev[min_left_sev_id].node[0].core_used += need_core;
		sev[min_left_sev_id].node[1].core_used += need_core;
		sev[min_left_sev_id].node[0].memory_used += need_memory;
		sev[min_left_sev_id].node[1].memory_used += need_memory;
		sev[min_left_sev_id].node[0].vma[sev[min_left_sev_id].node[0].number_vma].type = vma_type;
		sev[min_left_sev_id].node[0].vma[sev[min_left_sev_id].node[0].number_vma].vma_id = vma_id;
		sev[min_left_sev_id].node[0].number_vma++;
		add_list.sev_id[add_list.p] = min_left_sev_id; add_list.sev_node[add_list.p] = 2; add_list.p++;
		total_vma++;
		return 1;
	}
}

void add_vma_for_move(int vma_type, int vma_id, int sev_id, int sev_node)//��һ̨���ڵ㲿��������������һ̨��������һ���ڵ���
{
	if (sev[sev_id].total_core_used() + sev[sev_id].total_memory_used() == 0)total_idle_sev--;
	mp[vma_id] = sev_id + 1;
	sev[sev_id].node[sev_node].core_used += vmachine[vma_type].core;
	sev[sev_id].node[sev_node].memory_used += vmachine[vma_type].memory;
	sev[sev_id].node[sev_node].vma[sev[sev_id].node[sev_node].number_vma].type = vma_type;
	sev[sev_id].node[sev_node].vma[sev[sev_id].node[sev_node].number_vma].vma_id = vma_id;
	sev[sev_id].node[sev_node].number_vma++;
	total_vma++;
}
void add_vma_for_move_1(int  vma_type, int vma_id, int sev_id)//��һ̨˫�ڵ㲿��������������һ̨��������
{
	if (sev[sev_id].total_core_used() + sev[sev_id].total_memory_used() == 0)total_idle_sev--;
	mp[vma_id] = sev_id + 1;
	sev[sev_id].node[0].core_used += vmachine[vma_type].core / 2;
	sev[sev_id].node[0].memory_used += vmachine[vma_type].memory / 2;
	sev[sev_id].node[1].core_used += vmachine[vma_type].core / 2;
	sev[sev_id].node[1].memory_used += vmachine[vma_type].memory / 2;
	sev[sev_id].node[0].vma[sev[sev_id].node[0].number_vma].type = vma_type;
	sev[sev_id].node[0].vma[sev[sev_id].node[0].number_vma].vma_id = vma_id;
	sev[sev_id].node[0].number_vma++;
	total_vma++;
}

int vis_sev[100001];//0��ʾδ���ʹ���1��ʾ�����������̨������Ǩ������2��ʾ�������Ǩ�����̨������
vector<int> target_sev[100005];//��һ���Ǩ���У���¼�±�Ϊi�ķ���������Ǩ����Щ������
void move4() //��ͨǨ�Ʋ���
{
	for (int i = 0; i < total_sev; i++)
	{
		vis_sev[i] = 0; //�涨Ǩ��Ĳ�����Ǩ����Ǩ���Ŀ�����Ǩ��
		target_sev[i].clear();
	}
	int min_used_core = 1e8; int min_used_memory = 1e8; int min_used_sev_id; int min_used_sev_node;
	int used_core; int used_memory;
	int min_left_core = 1e8; int min_left_memory = 1e8; int min_left_sev_id; int min_left_sev_node;
	int need_core; int need_memory;
	int vma_id; int vma_type;
	int max_dailycost = 0; int day_cost;
	int min_dailycost = 1e8; int day_cost_move_to;
	double x = x3;//����
	double y = x4;
	for (daily_migration.W = 0; daily_migration.W < 3 * total_vma / 100;) //ÿ���Ǩ�ƴ��������ޣ������ܶ���õ�����
	{
		min_used_core = 1e8; min_used_memory = 1e8; int flag = 0;
		for (int i = 0; i < total_sev; i++) //�ҳ��и��صķ������и��رȽ��ٵģ����շ��ֱȽϸߵģ���ͼ��������������ϰ��ȥ
		{
			if (vis_sev[i] == 2)continue; //Ǩ��ķ�����������Ǩ��
			used_core = sev[i].node[0].core_used + sev[i].node[1].core_used;
			used_memory = sev[i].node[0].memory_used + sev[i].node[1].memory_used;
			day_cost = sever[sev[i].type].dailycost;
			if ((used_core != 0 || used_memory != 0) && used_core + used_memory - day_cost*y < min_used_core + min_used_memory - max_dailycost*y)
			{
				min_used_core = used_core;
				min_used_memory = used_memory;
				max_dailycost = day_cost;
				min_used_sev_id = i; flag = 1;
			}
		}
		if (flag == 0)break;
		//�ҵ����ʺϰ�յķ�������ѡ�������������������������������Ϊ���Ҹ��¼�
		if (sev[min_used_sev_id].node[0].number_vma>0)
		{
			vma_type = sev[min_used_sev_id].node[0].vma[sev[min_used_sev_id].node[0].number_vma - 1].type;
			vma_id = sev[min_used_sev_id].node[0].vma[sev[min_used_sev_id].node[0].number_vma - 1].vma_id;
			if (vmachine[vma_type].isbinode == 0)
			{
				need_core = vmachine[vma_type].core; need_memory = vmachine[vma_type].memory;
			}
			else if (vmachine[vma_type].isbinode == 1)
			{
				need_core = vmachine[vma_type].core / 2; need_memory = vmachine[vma_type].memory / 2;
			}
		}
		else if (sev[min_used_sev_id].node[1].number_vma > 0)
		{
			vma_type = sev[min_used_sev_id].node[1].vma[sev[min_used_sev_id].node[1].number_vma - 1].type;
			vma_id = sev[min_used_sev_id].node[1].vma[sev[min_used_sev_id].node[1].number_vma - 1].vma_id;
			need_core = vmachine[vma_type].core; need_memory = vmachine[vma_type].memory;
		}
		min_left_core = 1e8; min_left_memory = 1e8;
		if (vmachine[vma_type].isbinode == 0)//���������ǵ��ڵ㲿���
		{
			int flag1 = 0;
			for (int i = 0; i < target_sev[min_used_sev_id].size(); i++)//������̨������������Ŀ���������������ʵķ�����
			{
				int target_id = target_sev[min_used_sev_id][i];
				day_cost_move_to = sever[sev[target_id].type].dailycost;
				if (sev[target_id].node[0].core_left() >= need_core&&sev[target_id].node[0].memory_left() >= need_memory)
				{
					if (sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left() + sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left() + day_cost_move_to / x< min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left();
						min_left_memory = sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left();
						min_left_sev_id = target_id; min_left_sev_node = 0; flag1 = 1;
					}
				}
				if (sev[target_id].node[1].core_left() >= need_core&&sev[target_id].node[1].memory_left() >= need_memory)
				{
					if (sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left() + sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left() + day_cost_move_to / x< min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left();
						min_left_memory = sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left();
						min_left_sev_id = target_id; min_left_sev_node = 1; flag1 = 1;
					}
				}
			}
			if (flag1 != 0)//����ҵ���
			{
				vis_sev[min_left_sev_id] = 2;
				vis_sev[min_used_sev_id] = 1;
				daily_migration.sev_id[daily_migration.W] = min_left_sev_id;
				daily_migration.sev_node[daily_migration.W] = min_left_sev_node;
				daily_migration.vma_id[daily_migration.W] = vma_id;
				daily_migration.W++;
				del_vma(vma_id);
				add_vma_for_move(vma_type, vma_id, min_left_sev_id, min_left_sev_node);
				continue;
			}
			for (int i = 0; i < total_sev; i++)//����Ҳ���������װ������̨������ķ������У�ѡ��һ̨��Ҫ���˵ģ����շ��ֱȽϵ͵ķ���������Ϊ����������¼�
			{
				if (i == min_used_sev_id)continue;
				day_cost_move_to = sever[sev[i].type].dailycost;
				if (sev[i].node[0].core_left() >= need_core&&sev[i].node[0].memory_left() >= need_memory)
				{
					if (sev[i].node[0].core_left() + sev[i].node[1].core_left() + sev[i].node[0].memory_left() + sev[i].node[1].memory_left() + day_cost_move_to / x< min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[i].node[0].core_left() + sev[i].node[1].core_left();
						min_left_memory = sev[i].node[0].memory_left() + sev[i].node[1].memory_left();
						min_left_sev_id = i; min_left_sev_node = 0; flag1 = 1;
					}
				}
				if (sev[i].node[1].core_left() >= need_core&&sev[i].node[1].memory_left() >= need_memory)
				{
					if (sev[i].node[0].core_left() + sev[i].node[1].core_left() + sev[i].node[0].memory_left() + sev[i].node[1].memory_left() + day_cost_move_to / x< min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[i].node[0].core_left() + sev[i].node[1].core_left();
						min_left_memory = sev[i].node[0].memory_left() + sev[i].node[1].memory_left();
						min_left_sev_id = i; min_left_sev_node = 1; flag1 = 1;
					}
				}
			}
			if (flag1 == 0)break;
			target_sev[min_used_sev_id].push_back(min_left_sev_id);
			vis_sev[min_left_sev_id] = 2;
			vis_sev[min_used_sev_id] = 1;
			daily_migration.sev_id[daily_migration.W] = min_left_sev_id;
			daily_migration.sev_node[daily_migration.W] = min_left_sev_node;
			daily_migration.vma_id[daily_migration.W] = vma_id;
			daily_migration.W++;
			del_vma(vma_id);
			add_vma_for_move(vma_type, vma_id, min_left_sev_id, min_left_sev_node);
		}
		else if (vmachine[vma_type].isbinode == 1)//�����˫�ڵ㲿����������ͬ��
		{
			int flag1 = 0;
			for (int i = 0; i < target_sev[min_used_sev_id].size(); i++)
			{
				int target_id = target_sev[min_used_sev_id][i];
				day_cost_move_to = sever[sev[target_id].type].dailycost;
				if (sev[target_id].node[0].core_left() >= need_core&&sev[target_id].node[0].memory_left() >= need_memory&&sev[target_id].node[1].core_left() >= need_core&&sev[target_id].node[1].memory_left() >= need_memory)
				{
					if (sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left() + sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left() + day_cost_move_to / x < min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left();
						min_left_memory = sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left();
						min_left_sev_id = target_id; min_left_sev_node = 2; flag1 = 1;
					}
				}
			}
			if (flag1 != 0)
			{
				vis_sev[min_left_sev_id] = 2;
				vis_sev[min_used_sev_id] = 1;
				daily_migration.sev_id[daily_migration.W] = min_left_sev_id;
				daily_migration.sev_node[daily_migration.W] = min_left_sev_node;
				daily_migration.vma_id[daily_migration.W] = vma_id;
				daily_migration.W++;
				del_vma(vma_id);
				add_vma_for_move_1(vma_type, vma_id, min_left_sev_id);
				continue;
			}
			for (int i = 0; i < total_sev; i++)
			{
				if (i == min_used_sev_id)continue;
				day_cost_move_to = sever[sev[i].type].dailycost;
				if (sev[i].node[0].core_left() >= need_core&&sev[i].node[0].memory_left() >= need_memory&&sev[i].node[1].core_left() >= need_core&&sev[i].node[1].memory_left() >= need_memory)
				{
					if (sev[i].node[0].core_left() + sev[i].node[1].core_left() + sev[i].node[0].memory_left() + sev[i].node[1].memory_left() + day_cost_move_to / x < min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[i].node[0].core_left() + sev[i].node[1].core_left();
						min_left_memory = sev[i].node[0].memory_left() + sev[i].node[1].memory_left();
						min_left_sev_id = i; min_left_sev_node = 2; flag1 = 1;
					}
				}
			}
			if (flag1 == 0)break;
			target_sev[min_used_sev_id].push_back(min_left_sev_id);
			vis_sev[min_left_sev_id] = 2;
			vis_sev[min_used_sev_id] = 1;
			daily_migration.sev_id[daily_migration.W] = min_left_sev_id;
			daily_migration.sev_node[daily_migration.W] = min_left_sev_node;
			daily_migration.vma_id[daily_migration.W] = vma_id;
			daily_migration.W++;
			del_vma(vma_id);
			add_vma_for_move_1(vma_type, vma_id, min_left_sev_id);
		}
	}
}
void move7() //���ģ�����Ǩ�Ʋ���
{
	for (int i = 0; i < total_sev; i++)
	{
		vis_sev[i] = 0;//Ǩ��Ĳ�����Ǩ����Ǩ���Ŀ�����Ǩ��
		target_sev[i].clear();
	}
	int min_used_core = 1e8; int min_used_memory = 1e8; int min_used_sev_id; int min_used_sev_node;
	int used_core; int used_memory;
	int min_left_core = 1e8; int min_left_memory = 1e8; int min_left_sev_id; int min_left_sev_node;
	int need_core; int need_memory;
	int vma_id; int vma_type;
	int max_dailycost = 0; int day_cost;
	int min_dailycost = 1e8; int day_cost_move_to;
	double x = x3;//����
	double y = x4;
	for (daily_migration.W = 0; daily_migration.W < total_vma;) //������һ����д��ģ�������Դ���Ǩ��
	{
		min_used_core = 1e8; min_used_memory = 1e8; int flag = 0;
		for (int i = 0; i < total_sev; i++)//�ҳ��и��صķ������и��رȽ��ٵģ����շ��ֱȽϸߵģ���ͼ��������������ϰ��ȥ
		{
			if (vis_sev[i] == 2)continue;//Ǩ��ķ�����������Ǩ��
			used_core = sev[i].node[0].core_used + sev[i].node[1].core_used;
			used_memory = sev[i].node[0].memory_used + sev[i].node[1].memory_used;
			day_cost = sever[sev[i].type].dailycost;
			if ((used_core != 0 || used_memory != 0) && used_core + used_memory - day_cost*y < min_used_core + min_used_memory - max_dailycost*y)
			{
				min_used_core = used_core;
				min_used_memory = used_memory;
				max_dailycost = day_cost;
				min_used_sev_id = i; flag = 1;
			}
		}
		if (flag == 0)break;
		//�ҵ����ʺϰ�յķ�������ѡ�������������������������������Ϊ���Ҹ��¼�
		if (sev[min_used_sev_id].node[0].number_vma>0)
		{
			vma_type = sev[min_used_sev_id].node[0].vma[sev[min_used_sev_id].node[0].number_vma - 1].type;
			vma_id = sev[min_used_sev_id].node[0].vma[sev[min_used_sev_id].node[0].number_vma - 1].vma_id;
			if (vmachine[vma_type].isbinode == 0)
			{
				need_core = vmachine[vma_type].core; need_memory = vmachine[vma_type].memory;
			}
			else if (vmachine[vma_type].isbinode == 1)
			{
				need_core = vmachine[vma_type].core / 2; need_memory = vmachine[vma_type].memory / 2;
			}
		}
		else if (sev[min_used_sev_id].node[1].number_vma > 0)
		{
			vma_type = sev[min_used_sev_id].node[1].vma[sev[min_used_sev_id].node[1].number_vma - 1].type;
			vma_id = sev[min_used_sev_id].node[1].vma[sev[min_used_sev_id].node[1].number_vma - 1].vma_id;
			need_core = vmachine[vma_type].core; need_memory = vmachine[vma_type].memory;
		}
		min_left_core = 1e8; min_left_memory = 1e8;
		if (vmachine[vma_type].isbinode == 0)//���������ǵ��ڵ㲿���
		{
			int flag1 = 0;
			for (int i = 0; i < target_sev[min_used_sev_id].size(); i++)//������̨������������Ŀ���������������ʵķ�����
			{
				int target_id = target_sev[min_used_sev_id][i];
				day_cost_move_to = sever[sev[target_id].type].dailycost;
				if (sev[target_id].node[0].core_left() >= need_core&&sev[target_id].node[0].memory_left() >= need_memory)
				{
					if (sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left() + sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left() + day_cost_move_to / x< min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left();
						min_left_memory = sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left();
						min_left_sev_id = target_id; min_left_sev_node = 0; flag1 = 1;
					}
				}
				if (sev[target_id].node[1].core_left() >= need_core&&sev[target_id].node[1].memory_left() >= need_memory)
				{
					if (sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left() + sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left() + day_cost_move_to / x< min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left();
						min_left_memory = sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left();
						min_left_sev_id = target_id; min_left_sev_node = 1; flag1 = 1;
					}
				}
			}
			if (flag1 != 0)//����ҵ���
			{
				vis_sev[min_left_sev_id] = 2;
				vis_sev[min_used_sev_id] = 1;
				daily_migration.sev_id[daily_migration.W] = min_left_sev_id;
				daily_migration.sev_node[daily_migration.W] = min_left_sev_node;
				daily_migration.vma_id[daily_migration.W] = vma_id;
				daily_migration.W++;
				del_vma(vma_id);
				add_vma_for_move(vma_type, vma_id, min_left_sev_id, min_left_sev_node);
				continue;
			}
			for (int i = 0; i < total_sev; i++)//����Ҳ���������װ������̨������ķ������У�ѡ��һ̨��Ҫ���˵ģ����շ��ֱȽϵ͵ķ���������Ϊ����������¼�
			{
				if (i == min_used_sev_id)continue;
				day_cost_move_to = sever[sev[i].type].dailycost;
				if (sev[i].node[0].core_left() >= need_core&&sev[i].node[0].memory_left() >= need_memory)
				{
					if (sev[i].node[0].core_left() + sev[i].node[1].core_left() + sev[i].node[0].memory_left() + sev[i].node[1].memory_left() + day_cost_move_to / x< min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[i].node[0].core_left() + sev[i].node[1].core_left();
						min_left_memory = sev[i].node[0].memory_left() + sev[i].node[1].memory_left();
						min_left_sev_id = i; min_left_sev_node = 0; flag1 = 1;
					}
				}
				if (sev[i].node[1].core_left() >= need_core&&sev[i].node[1].memory_left() >= need_memory)
				{
					if (sev[i].node[0].core_left() + sev[i].node[1].core_left() + sev[i].node[0].memory_left() + sev[i].node[1].memory_left() + day_cost_move_to / x< min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[i].node[0].core_left() + sev[i].node[1].core_left();
						min_left_memory = sev[i].node[0].memory_left() + sev[i].node[1].memory_left();
						min_left_sev_id = i; min_left_sev_node = 1; flag1 = 1;
					}
				}
			}
			if (flag1 == 0)break;
			target_sev[min_used_sev_id].push_back(min_left_sev_id);
			vis_sev[min_left_sev_id] = 2;
			vis_sev[min_used_sev_id] = 1;
			daily_migration.sev_id[daily_migration.W] = min_left_sev_id;
			daily_migration.sev_node[daily_migration.W] = min_left_sev_node;
			daily_migration.vma_id[daily_migration.W] = vma_id;
			daily_migration.W++;
			del_vma(vma_id);
			add_vma_for_move(vma_type, vma_id, min_left_sev_id, min_left_sev_node);
		}
		else if (vmachine[vma_type].isbinode == 1)//�����˫�ڵ㲿����������ͬ��
		{
			int flag1 = 0;
			for (int i = 0; i < target_sev[min_used_sev_id].size(); i++)
			{
				int target_id = target_sev[min_used_sev_id][i];
				day_cost_move_to = sever[sev[target_id].type].dailycost;
				if (sev[target_id].node[0].core_left() >= need_core&&sev[target_id].node[0].memory_left() >= need_memory&&sev[target_id].node[1].core_left() >= need_core&&sev[target_id].node[1].memory_left() >= need_memory)
				{
					if (sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left() + sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left() + day_cost_move_to / x < min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left();
						min_left_memory = sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left();
						min_left_sev_id = target_id; min_left_sev_node = 2; flag1 = 1;
					}
				}
			}
			if (flag1 != 0)
			{
				vis_sev[min_left_sev_id] = 2;
				vis_sev[min_used_sev_id] = 1;
				daily_migration.sev_id[daily_migration.W] = min_left_sev_id;
				daily_migration.sev_node[daily_migration.W] = min_left_sev_node;
				daily_migration.vma_id[daily_migration.W] = vma_id;
				daily_migration.W++;
				del_vma(vma_id);
				add_vma_for_move_1(vma_type, vma_id, min_left_sev_id);
				continue;
			}
			for (int i = 0; i < total_sev; i++)
			{
				if (i == min_used_sev_id)continue;
				day_cost_move_to = sever[sev[i].type].dailycost;
				if (sev[i].node[0].core_left() >= need_core&&sev[i].node[0].memory_left() >= need_memory&&sev[i].node[1].core_left() >= need_core&&sev[i].node[1].memory_left() >= need_memory)
				{
					if (sev[i].node[0].core_left() + sev[i].node[1].core_left() + sev[i].node[0].memory_left() + sev[i].node[1].memory_left() + day_cost_move_to / x < min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[i].node[0].core_left() + sev[i].node[1].core_left();
						min_left_memory = sev[i].node[0].memory_left() + sev[i].node[1].memory_left();
						min_left_sev_id = i; min_left_sev_node = 2; flag1 = 1;
					}
				}
			}
			if (flag1 == 0)break;
			target_sev[min_used_sev_id].push_back(min_left_sev_id);
			vis_sev[min_left_sev_id] = 2;
			vis_sev[min_used_sev_id] = 1;
			daily_migration.sev_id[daily_migration.W] = min_left_sev_id;
			daily_migration.sev_node[daily_migration.W] = min_left_sev_node;
			daily_migration.vma_id[daily_migration.W] = vma_id;
			daily_migration.W++;
			del_vma(vma_id);
			add_vma_for_move_1(vma_type, vma_id, min_left_sev_id);
		}
	}
}
void from_sev_to_sev(int from_sev_id, int to_sev_id, int&left_migration_times) //�԰��������һ̨�������������ȫ��Ǩ�Ƶ���һ̨��������
{
	while (sev[from_sev_id].node[0].number_vma > 0)
	{
		int vma_id = sev[from_sev_id].node[0].vma[sev[from_sev_id].node[0].number_vma - 1].vma_id;
		int vma_type = sev[from_sev_id].node[0].vma[sev[from_sev_id].node[0].number_vma - 1].type;
		daily_migration.sev_id[daily_migration.W] = to_sev_id;
		if (vmachine[vma_type].isbinode == 0)
		{
			daily_migration.sev_node[daily_migration.W] = 0;
			daily_migration.vma_id[daily_migration.W] = vma_id;
			daily_migration.W++;
			del_vma(vma_id);
			add_vma_for_move(vma_type, vma_id, to_sev_id, 0);
			left_migration_times--;
		}
		else if (vmachine[vma_type].isbinode == 1)//˫�ڵ㲿��������
		{
			daily_migration.sev_node[daily_migration.W] = 2;
			daily_migration.vma_id[daily_migration.W] = vma_id;
			daily_migration.W++;
			del_vma(vma_id);
			add_vma_for_move_1(vma_type, vma_id, to_sev_id);
			left_migration_times--;
		}
	}
	while (sev[from_sev_id].node[1].number_vma > 0)
	{
		int vma_id = sev[from_sev_id].node[1].vma[sev[from_sev_id].node[1].number_vma - 1].vma_id;
		int vma_type = sev[from_sev_id].node[1].vma[sev[from_sev_id].node[1].number_vma - 1].type;
		daily_migration.sev_id[daily_migration.W] = to_sev_id;
		daily_migration.sev_node[daily_migration.W] = 1;
		daily_migration.vma_id[daily_migration.W] = vma_id;
		daily_migration.W++;
		del_vma(vma_id);
		add_vma_for_move(vma_type, vma_id, to_sev_id, 1);
		left_migration_times--;
	}
}
int vis_sev1[100001];
void move5()//ʹ�ö԰������Ǩ�Ʋ��ԣ�����Ч��������
{
	for (int i = 0; i < total_sev; i++)
	{
		vis_sev[i] = 0;//Ǩ��Ĳ�����Ǩ����Ǩ���Ĳ�����Ǩ��
		vis_sev1[i] = 0;
		target_sev[i].clear();
	}
	int min_used_core = 1e8; int min_used_memory = 1e8; int min_used_sev_id; int min_used_sev_node;
	int used_core; int used_memory;
	int min_left_core = 1e8; int min_left_memory = 1e8; int min_left_sev_id; int min_left_sev_node;
	int need_core; int need_memory;
	int vma_id; int vma_type;
	int max_dailycost = 0; int day_cost;
	int min_dailycost = 1e8; int day_cost_move_to;
	double x = x3;//����
	double y = x4;
	daily_migration.W = 0;
	int left_migration_times = total_vma - daily_migration.W;
	int used_core0; int used_core1; int used_memory0; int used_memory1;
	int left_core0; int left_core1; int left_memory0; int left_memory1;
	double y1 = y; double y2 = x;
	for (; daily_migration.W < total_vma;)
	{
		min_used_core = 1e8; min_used_memory = 1e8; int flag = 0; max_dailycost = 0;
		for (int i = 0; i < total_sev; i++)
		{
			if (sev[i].total_number_vma()>left_migration_times)continue;
			if (vis_sev1[i] == 2)continue;
			used_core = sev[i].node[0].core_used + sev[i].node[1].core_used;
			used_memory = sev[i].node[0].memory_used + sev[i].node[1].memory_used;
			day_cost = sever[sev[i].type].dailycost;
			if ((used_core != 0 || used_memory != 0) && used_core + used_memory < min_used_core + min_used_memory)
			{
				min_used_core = used_core;
				min_used_memory = used_memory;
				max_dailycost = day_cost;
				min_used_sev_id = i; flag = 1;
			}
		}
		if (flag == 0)break;
		int flag1 = 0;
		used_core0 = sev[min_used_sev_id].node[0].core_used; used_memory0 = sev[min_used_sev_id].node[0].memory_used;
		used_core1 = sev[min_used_sev_id].node[1].core_used; used_memory1 = sev[min_used_sev_id].node[1].memory_used;
		min_left_core = 1e8; min_left_memory = 1e8; min_dailycost = 1e8;
		for (int i = 0; i < total_sev; i++)
		{
			if (i == min_used_sev_id)continue;
			if (sev[i].total_core_used() + sev[i].total_memory_used() == 0)continue;
			day_cost_move_to = sever[sev[i].type].dailycost;
			if (sev[i].node[0].core_left() >= used_core0&&sev[i].node[0].memory_left() >= used_memory0&&sev[i].node[1].core_left() >= used_core1&&sev[i].node[1].memory_left() >= used_memory1)
			{
				if (day_cost_move_to / y2<  min_dailycost / y2)
				{
					min_dailycost = day_cost_move_to;
					min_left_core = sev[i].node[0].core_left() + sev[i].node[1].core_left();
					min_left_memory = sev[i].node[0].memory_left() + sev[i].node[1].memory_left();
					min_left_sev_id = i;  flag1 = 1;
				}
			}
		}
		if (flag1 == 0)
		{
			min_left_core = 1e8; min_left_memory = 1e8; min_dailycost = 1e8;
			for (int i = 0; i < total_sev; i++)
			{
				if (i == min_used_sev_id)continue;
				if (sev[i].total_core_used() + sev[i].total_memory_used() != 0)continue;
				day_cost_move_to = sever[sev[i].type].dailycost;
				if (sev[i].node[0].core_left() >= used_core0&&sev[i].node[0].memory_left() >= used_memory0&&sev[i].node[1].core_left() >= used_core1&&sev[i].node[1].memory_left() >= used_memory1)
				{
					if (day_cost_move_to<min_dailycost)
					{
						min_dailycost = day_cost_move_to;
						min_left_sev_id = i;  flag1 = 1;
					}
				}
			}
		}
		if (flag1 == 0)break;
		vis_sev1[min_left_sev_id] = 2;
		from_sev_to_sev(min_used_sev_id, min_left_sev_id, left_migration_times);
	}
	for (; daily_migration.W < total_vma;)
	{
		min_used_core = 1e8; min_used_memory = 1e8; int flag = 0;
		for (int i = 0; i < total_sev; i++)//�ҳ��и��صķ������и��رȽ��ٵģ����շ��ֱȽϸߵģ���ͼ��������������ϰ��ȥ
		{
			if (vis_sev[i] == 2)continue;//Ǩ��ķ�����������Ǩ��
			used_core = sev[i].node[0].core_used + sev[i].node[1].core_used;
			used_memory = sev[i].node[0].memory_used + sev[i].node[1].memory_used;
			day_cost = sever[sev[i].type].dailycost;
			if ((used_core != 0 || used_memory != 0) && used_core + used_memory - day_cost*y < min_used_core + min_used_memory - max_dailycost*y)
			{
				min_used_core = used_core;
				min_used_memory = used_memory;
				max_dailycost = day_cost;
				min_used_sev_id = i; flag = 1;
			}
		}
		if (flag == 0)break;
		//�ҵ����ʺϰ�յķ�������ѡ�������������������������������Ϊ���Ҹ��¼�
		if (sev[min_used_sev_id].node[0].number_vma>0)
		{
			vma_type = sev[min_used_sev_id].node[0].vma[sev[min_used_sev_id].node[0].number_vma - 1].type;
			vma_id = sev[min_used_sev_id].node[0].vma[sev[min_used_sev_id].node[0].number_vma - 1].vma_id;
			if (vmachine[vma_type].isbinode == 0)
			{
				need_core = vmachine[vma_type].core; need_memory = vmachine[vma_type].memory;
			}
			else if (vmachine[vma_type].isbinode == 1)
			{
				need_core = vmachine[vma_type].core / 2; need_memory = vmachine[vma_type].memory / 2;
			}
		}
		else if (sev[min_used_sev_id].node[1].number_vma > 0)
		{
			vma_type = sev[min_used_sev_id].node[1].vma[sev[min_used_sev_id].node[1].number_vma - 1].type;
			vma_id = sev[min_used_sev_id].node[1].vma[sev[min_used_sev_id].node[1].number_vma - 1].vma_id;
			need_core = vmachine[vma_type].core; need_memory = vmachine[vma_type].memory;
		}
		min_left_core = 1e8; min_left_memory = 1e8;
		if (vmachine[vma_type].isbinode == 0)//���������ǵ��ڵ㲿���
		{
			int flag1 = 0;
			for (int i = 0; i < target_sev[min_used_sev_id].size(); i++)//������̨������������Ŀ���������������ʵķ�����
			{
				int target_id = target_sev[min_used_sev_id][i];
				day_cost_move_to = sever[sev[target_id].type].dailycost;
				if (sev[target_id].node[0].core_left() >= need_core&&sev[target_id].node[0].memory_left() >= need_memory)
				{
					if (sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left() + sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left() + day_cost_move_to / x< min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left();
						min_left_memory = sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left();
						min_left_sev_id = target_id; min_left_sev_node = 0; flag1 = 1;
					}
				}
				if (sev[target_id].node[1].core_left() >= need_core&&sev[target_id].node[1].memory_left() >= need_memory)
				{
					if (sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left() + sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left() + day_cost_move_to / x< min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left();
						min_left_memory = sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left();
						min_left_sev_id = target_id; min_left_sev_node = 1; flag1 = 1;
					}
				}
			}
			if (flag1 != 0)//����ҵ���
			{
				vis_sev[min_left_sev_id] = 2;
				vis_sev[min_used_sev_id] = 1;
				daily_migration.sev_id[daily_migration.W] = min_left_sev_id;
				daily_migration.sev_node[daily_migration.W] = min_left_sev_node;
				daily_migration.vma_id[daily_migration.W] = vma_id;
				daily_migration.W++;
				del_vma(vma_id);
				add_vma_for_move(vma_type, vma_id, min_left_sev_id, min_left_sev_node);
				continue;
			}
			for (int i = 0; i < total_sev; i++)//����Ҳ���������װ������̨������ķ������У�ѡ��һ̨��Ҫ���˵ģ����շ��ֱȽϵ͵ķ���������Ϊ����������¼�
			{
				if (i == min_used_sev_id)continue;
				day_cost_move_to = sever[sev[i].type].dailycost;
				if (sev[i].node[0].core_left() >= need_core&&sev[i].node[0].memory_left() >= need_memory)
				{
					if (sev[i].node[0].core_left() + sev[i].node[1].core_left() + sev[i].node[0].memory_left() + sev[i].node[1].memory_left() + day_cost_move_to / x< min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[i].node[0].core_left() + sev[i].node[1].core_left();
						min_left_memory = sev[i].node[0].memory_left() + sev[i].node[1].memory_left();
						min_left_sev_id = i; min_left_sev_node = 0; flag1 = 1;
					}
				}
				if (sev[i].node[1].core_left() >= need_core&&sev[i].node[1].memory_left() >= need_memory)
				{
					if (sev[i].node[0].core_left() + sev[i].node[1].core_left() + sev[i].node[0].memory_left() + sev[i].node[1].memory_left() + day_cost_move_to / x< min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[i].node[0].core_left() + sev[i].node[1].core_left();
						min_left_memory = sev[i].node[0].memory_left() + sev[i].node[1].memory_left();
						min_left_sev_id = i; min_left_sev_node = 1; flag1 = 1;
					}
				}
			}
			if (flag1 == 0)break;
			target_sev[min_used_sev_id].push_back(min_left_sev_id);
			vis_sev[min_left_sev_id] = 2;
			vis_sev[min_used_sev_id] = 1;
			daily_migration.sev_id[daily_migration.W] = min_left_sev_id;
			daily_migration.sev_node[daily_migration.W] = min_left_sev_node;
			daily_migration.vma_id[daily_migration.W] = vma_id;
			daily_migration.W++;
			del_vma(vma_id);
			add_vma_for_move(vma_type, vma_id, min_left_sev_id, min_left_sev_node);
		}
		else if (vmachine[vma_type].isbinode == 1)//�����˫�ڵ㲿����������ͬ��
		{
			int flag1 = 0;
			for (int i = 0; i < target_sev[min_used_sev_id].size(); i++)
			{
				int target_id = target_sev[min_used_sev_id][i];
				day_cost_move_to = sever[sev[target_id].type].dailycost;
				if (sev[target_id].node[0].core_left() >= need_core&&sev[target_id].node[0].memory_left() >= need_memory&&sev[target_id].node[1].core_left() >= need_core&&sev[target_id].node[1].memory_left() >= need_memory)
				{
					if (sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left() + sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left() + day_cost_move_to / x < min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[target_id].node[0].core_left() + sev[target_id].node[1].core_left();
						min_left_memory = sev[target_id].node[0].memory_left() + sev[target_id].node[1].memory_left();
						min_left_sev_id = target_id; min_left_sev_node = 2; flag1 = 1;
					}
				}
			}
			if (flag1 != 0)
			{
				vis_sev[min_left_sev_id] = 2;
				vis_sev[min_used_sev_id] = 1;
				daily_migration.sev_id[daily_migration.W] = min_left_sev_id;
				daily_migration.sev_node[daily_migration.W] = min_left_sev_node;
				daily_migration.vma_id[daily_migration.W] = vma_id;
				daily_migration.W++;
				del_vma(vma_id);
				add_vma_for_move_1(vma_type, vma_id, min_left_sev_id);
				continue;
			}
			for (int i = 0; i < total_sev; i++)
			{
				if (i == min_used_sev_id)continue;
				day_cost_move_to = sever[sev[i].type].dailycost;
				if (sev[i].node[0].core_left() >= need_core&&sev[i].node[0].memory_left() >= need_memory&&sev[i].node[1].core_left() >= need_core&&sev[i].node[1].memory_left() >= need_memory)
				{
					if (sev[i].node[0].core_left() + sev[i].node[1].core_left() + sev[i].node[0].memory_left() + sev[i].node[1].memory_left() + day_cost_move_to / x < min_left_core + min_left_memory + min_dailycost / x)
					{
						min_dailycost = day_cost_move_to;
						min_left_core = sev[i].node[0].core_left() + sev[i].node[1].core_left();
						min_left_memory = sev[i].node[0].memory_left() + sev[i].node[1].memory_left();
						min_left_sev_id = i; min_left_sev_node = 2; flag1 = 1;
					}
				}
			}
			if (flag1 == 0)break;
			target_sev[min_used_sev_id].push_back(min_left_sev_id);
			vis_sev[min_left_sev_id] = 2;
			vis_sev[min_used_sev_id] = 1;
			daily_migration.sev_id[daily_migration.W] = min_left_sev_id;
			daily_migration.sev_node[daily_migration.W] = min_left_sev_node;
			daily_migration.vma_id[daily_migration.W] = vma_id;
			daily_migration.W++;
			del_vma(vma_id);
			add_vma_for_move_1(vma_type, vma_id, min_left_sev_id);
		}
	}
}
int main()
{
	//std::ios_base::sync_with_stdio(false);
	//cin.tie(NULL); cout.tie(NULL);
	//�Լ����ص���ʱ�����Զ�ȡ���ݼ���������������������ʱ��Ҫע�͵�
	//	freopen("training-1.txt", "r", stdin);
	//	freopen("ou1.txt", "w", stdout);
	int flag_migration = 0;
	cin >> N;
	char name[24]; int a, b, e; LL c, d; char opt[10]; int id;
	for (int i = 0; i < N; i++)
	{
		while (cin.get() != '(');
		cin >> name; cin >> a; cin.get(); cin >> b; cin.get(); cin >> c; cin.get(); cin >> d; cin.get();
		sever[i].update(name, a, b, c, d);
	}
	cin >> M;
	for (int i = 0; i < M; i++)
	{
		while (cin.get() != '(');
		cin >> name; cin >> a; cin.get(); cin >> b; cin.get(); cin >> e; cin.get();
		vmachine[i].update(name, a, b, e);
	}
	cin >> T; cin >> K;
	for (int i = 0; i < T; i++)//����һ�����һ��
	{
		CPU_core_demand = 1;
		instorage_demand = 1;
		cin >> R;
		daily_opt.top = 0; daily_migration.W = 0; add_list.p = 0; buy_list.init();
		int left_day = T - i;
		for (int j = 0; j < R; j++)//�Ѷ����һ������󶼼�¼�ڱ�
		{
			while (cin.get() != '(');
			cin >> opt;
			if (strcmp1(opt, "add,") == 1)
			{
				daily_opt.opt_type[daily_opt.top] = 1;
				cin >> name;
				for (int i = 0; i < M; i++)
				{
					if (strcmp1(name, vmachine[i].model) == 1)
					{
						daily_opt.vma_type[daily_opt.top] = i; break;
					}
				}
				cin >> id;
				daily_opt.vma_id[daily_opt.top] = id;
				daily_opt.top++;
				cin.get();
			}
			else if (strcmp1(opt, "del,") == 1)
			{
				daily_opt.opt_type[daily_opt.top] = 2;
				cin >> id;
				daily_opt.vma_id[daily_opt.top] = id;
				daily_opt.top++;
				cin.get();
			}
		}



		//Ǩ��
		//������Բ�ͬ�����ʵ����ʹ�ò�ͬ��Ǩ�Ʋ��ԣ���Ч������
		if (flag_migration == 0 && i>T / 3 && idle_rate()>0.5)
		{
			move5();
			flag_migration = 1;
		}
		else
		{
			move4();
		}
		//ģ�ⲿ��͹���
		for (int i = 0; i < daily_opt.top; i++) //һ�����������󲢼�¼�������¾��򲢼�¼
		{
			if (daily_opt.opt_type[i] == 1)
			{
				int successfully_deployed; //����ɹ���־
				if (idle_rate() < 0.4) //������Բ�ͬ�����ʵ����ʹ�ò�ͬ�Ĳ�����ԣ���Ч������
				{
					successfully_deployed = add_vma(daily_opt.vma_type[i], daily_opt.vma_id[i]);
				}
				else
				{
					successfully_deployed = add_vma(daily_opt.vma_type[i], daily_opt.vma_id[i]);
				}
				if (successfully_deployed == 0) //���𲻳ɹ�����ʾ��Ҫ����
				{
					int flag_buy = 0; double buy_x1 = 1.005; double buy_x2 = 1.8;
					double min_evaluate = 1e16; int min_evaluate_sev_type;
					int need_core = vmachine[daily_opt.vma_type[i]].core;
					int need_memory = vmachine[daily_opt.vma_type[i]].memory;
					if (vmachine[daily_opt.vma_type[i]].isbinode == 0) //���ڵ㲿��
					{
						//���ܹ�װ�������������ķ������У�ѡĿǰ��/�Ա���͵�һ̨������
						for (int j = 0; j < N; j++)
						{
							if (sever[j].core / 2 >= need_core&&sever[j].memory / 2 >= need_memory)
							{
								double evaluate_value = sever[j].evaluate(left_day);
								if (evaluate_value < min_evaluate)
								{
									min_evaluate = evaluate_value; flag_buy = 1;
									min_evaluate_sev_type = j;
								}
							}
						}
					}
					else if (vmachine[daily_opt.vma_type[i]].isbinode == 1) //˫��㲿��
					{
						for (int j = 0; j < N; j++)
						{
							if (sever[j].core >= need_core&&sever[j].memory >= need_memory)
							{
								double evaluate_value = sever[j].evaluate(left_day);
								if (evaluate_value < min_evaluate)
								{
									min_evaluate = evaluate_value; flag_buy = 1;
									min_evaluate_sev_type = j;
								}
							}
						}
					}
					if (buy_list.type_buy_number[min_evaluate_sev_type] == 0)
					{
						buy_list.Q++;
						buy_list.type_buy_number[min_evaluate_sev_type] = 1;
					}
					else
					{
						buy_list.type_buy_number[min_evaluate_sev_type]++;
					}


					sev[total_sev].type = min_evaluate_sev_type;
					sev[total_sev].node[0].core_all = sever[min_evaluate_sev_type].core / 2;
					sev[total_sev].node[0].memory_all = sever[min_evaluate_sev_type].memory / 2;
					sev[total_sev].node[1].core_all = sever[min_evaluate_sev_type].core / 2;
					sev[total_sev].node[1].memory_all = sever[min_evaluate_sev_type].memory / 2;
					total_sev++; total_idle_sev++;
					i--;
				}
			}
			else if (daily_opt.opt_type[i] == 2)
			{
				del_vma(daily_opt.vma_id[i]);
			}
		}
		//cout << "������Ϊ��" << 1.0*total_idle_sev / total_sev << "\n";
		//������֮ǰ��ֱ����Ϊ��������id==sev[]���±꼴�ɣ�������֮��Ҳû�п����Ż��ĵط�
		cout << "(purchase, " << buy_list.Q << ")\n";
		for (int i = 0; i < 101; i++)
		{
			if (buy_list.type_buy_number[i]>0)
			{
				cout << "(" << sever[i].model << " " << buy_list.type_buy_number[i] << ")\n";
				for (int j = 0; j < buy_list.type_buy_number[i]; j++)
				{

					for (int k = total_sev - 1; k >= 0; k--)
					{
						if (sev[k].type == i&&mp2[k + 1] == 0)
						{
							mp2[k + 1] = total_sev1 + 1; break;  //�洢��sev[]�±꣬���˳���ţ���ֵ��
						}
					}
					total_sev1++;
				}
			}
		}


		cout << "(migration, " << daily_migration.W << ")\n";
		for (int i = 0; i < daily_migration.W; i++)
		{
			if (daily_migration.sev_node[i] == 0)
			{
				cout << "(" << daily_migration.vma_id[i] << ", " << mp2[daily_migration.sev_id[i] + 1] - 1 << ", A)\n"; //���������idǰ����ת���������ı��
			}
			else if (daily_migration.sev_node[i] == 1)
			{
				cout << "(" << daily_migration.vma_id[i] << ", " << mp2[daily_migration.sev_id[i] + 1] - 1 << ", B)\n";
			}
			else if (daily_migration.sev_node[i] == 2)
			{
				cout << "(" << daily_migration.vma_id[i] << ", " << mp2[daily_migration.sev_id[i] + 1] - 1 << ")\n";
			}
		}



		for (int i = 0; i < add_list.p; i++)//��ÿ������������������������ʲô��ţ������ı�ţ���������Ϊ�������ŵı�ţ���������� 
		{

			if (add_list.sev_node[i] == 0)
			{
				cout << "(" << mp2[add_list.sev_id[i] + 1] - 1 << ", A)\n";
			}
			else if (add_list.sev_node[i] == 1)
			{
				cout << "(" << mp2[add_list.sev_id[i] + 1] - 1 << ", B)\n";
			}
			else if (add_list.sev_node[i] == 2)
			{
				cout << "(" << mp2[add_list.sev_id[i] + 1] - 1 << ")\n";
			}
		}
		fflush(stdout);
	}

}