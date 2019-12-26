#include <iostream>
#include <map>
#include <cstdio>
#include <cstring>

using namespace std;

class warrior* C[25][2];
int M,N,R,K,T;
int chp[25],flag[25],lastwin[25],win[25];
bool arr[25];
map<string,int> HP,ATK;
string warrior_type[2][5]={
	{"iceman","lion","wolf","ninja","dragon"},
	{"lion","dragon","ninja","iceman","wolf"},
};

class weapon{
    public:
	int atk;
	weapon(int type,int _atk){
		if(type==0) atk=_atk/5;
		if(type==1) atk=100;
		if(type==2) atk=3;
	}
};

class warrior{
    public:
	int atk;
	int hp;
	int id;
	int loyalty;
	int step;
	string color;
	string type;
	double morale;
	weapon* wea[3];
	warrior(string _color,int _id,string _type,double r_hp):color(_color),type(_type),id(_id){
		step=morale=0;
		loyalty=100;
		hp=HP[type];
		atk=ATK[type];		 
		memset(wea,0 ,sizeof(wea));
		auto WA=new weapon(id%3,atk),WB=new weapon((id+1)%3,atk);
		if(WA->atk==0) WA=NULL;
		if(WB->atk==0) WB=NULL;
		if(type=="dragon"){
			wea[id%3]=WA;
			morale=(double)r_hp/HP["dragon"];
		}
		if(type=="ninja") wea[id%3]=WA,wea[(id+1)%3]=WB;
		if(type=="iceman") wea[id%3]=WA;
		if(type=="lion") loyalty=r_hp;
	}
	string int_to_string(int x){
		string str="";
		while(x){
		    str=char(x%10+'0')+str;
			x/=10;
		}
		return str;
	}
	operator string (){
		return color+' '+type+' '+int_to_string(id);
	}
	string info(){
		return "with "+int_to_string(hp)+" elements and force "+int_to_string(atk);
	}
	int usesword(bool con){
		auto &x=wea[0];
		if(!x) return 0;
		int res=x->atk;
		if(con){
			x->atk=(int)(x->atk*0.8);
			if(!x->atk) x=NULL;
		}
		return res;
	}
	int getatk(bool defense){
		if(defense) return atk/2+usesword(0);
		else return atk+usesword(0);
	}
};

class Game{
    public:
	int t;
	void reset(int num);
	void turn();
	void escape();
	void march();
	void gethp();
	void shoot();
	void usebomb();
	void war();
	void print(warrior);
}game;

class headquarter{
    public:
	int hp;
	int tot;
	int type;
	int location;
	int rew;
	string color;
	headquarter(){}
	headquarter(string _color,int _hp):color(_color),hp(_hp){
		if(color=="red") type=0,location=0;
		else type=1,location=N+1;
		tot=0;
	}
	void born(){
		string Stype=warrior_type[type][tot%5];
		if(HP[Stype]<=hp){
			hp-=HP[Stype];
			tot++;
			warrior* S=new warrior(color,tot,Stype,hp);
			C[location][type]=S;
			printf("%03d:00 %s born\n",game.t/60,string(*S).c_str());
			if(S->type=="dragon")
				printf("Its morale is %.2lf\n",S->morale);
			if(S->type=="lion")
				printf("Its loyalty is %d\n",S->loyalty);
		}
	}
}H[2];

void Game::reset(int num){
	printf("Case %d:\n",num);
	t=0;
	for(int i=0;i<=N+1;i++){
		C[i][0]=C[i][1]=NULL;
		flag[i]=lastwin[i]=-1;
		chp[i]=0;
	}
}
void Game::escape(){
	for(int i=0;i<=N+1;i++)
		for(int j=0;j<2;j++)
			if(C[i][j]&&C[i][j]->loyalty<=0&&i!=H[1-j].location){
				printf("%03d:05 %s ran away\n",game.t/60,string(*C[i][j]).c_str()),
				C[i][j]=NULL;
			}
}
void Game::march(){
	bool end=false;
	int d[2]={-1,1};
	warrior *tmp[25][2]={NULL};
	for(int i=0;i<=N+1;i++)		   
		for(int j=0;j<2;j++){
			int u=i+d[j];
			if(i==H[1-j].location) tmp[i][j]=C[i][j];
			if(C[u][j]){
				if(u<0||u>N+1)continue;
				warrior *S=C[u][j];
				if(!(++S->step&1)&&S->type=="iceman") S->atk+=20,S->hp=max(1,S->hp-9);
				tmp[i][j]=S;
				if(i==H[1-j].location){
					printf("%03d:10 %s reached %s headquarter %s\n",game.t/60,string(*S).c_str(),H[j^1].color.c_str(),S->info().c_str());
					if(C[i][j]){
						printf("%03d:10 %s headquarter was taken\n",game.t/60,H[j^1].color.c_str());
						end=true;
					}
				}
				else printf("%03d:10 %s marched to city %d %s\n",game.t/60,string(*S).c_str(),i,S->info().c_str());
			}
		}
	for(int i=0;i<=N+1;i++)
		for(int j=0;j<2;j++)
			C[i][j]=tmp[i][j];
	if(end) game.t=-1;
}
void Game::gethp(){
	for(int i=1;i<=N ;i++)
		for(int j=0;j<2;j++)
			if(C[i][j]&&!C[i][1-j]){
				H[j].hp+=chp[i];
				printf("%03d:30 %s earned %d elements for his headquarter\n",game.t/60,string(*C[i][j]).c_str(),chp[i]);
				chp[i]=0;
			}
}
void Game::shoot(){
	memset(arr,0,sizeof(arr));
	for(int i=0;i<=N+1;i++)
		for(int j=0;j<2;j++){
			warrior *u=C[i][j],*v=NULL;
			int p;
			if(j==0) p=i+1;
			else p=i-1;
			if(p>=0&&p<=N+1) v=C[p][1-j];
			if(!u||!v||!u->wea[2]) continue;
			v->hp-=R;
			u->wea[2]->atk--;
			if(!u->wea[2]->atk)u->wea[2]=NULL;
			if(v->hp>0) printf("%03d:35 %s shot\n",game.t/60,string(*u).c_str());
			else{
				printf("%03d:35 %s shot and killed %s\n",game.t/60,string(*u).c_str(),string(*v).c_str());
				arr[p]=1;
			}
		}
}
void Game::usebomb(){
	for(int i=1;i<=N;i++){
        int p;
		if(flag[i]!=-1) p=flag[i];
		else p=!(i&1);
		auto &u=C[i][p],&v=C[i][1-p];
		if(!u||!v||arr[i])continue;
		if(v->wea[1]&&u->getatk(0)>=v->hp){
			printf("%03d:38 %s used a bomb and killed %s\n",game.t/60,string(*v).c_str(),string(*u).c_str());
			u=v=NULL;
		}
		else if(u->wea[1]&&u->getatk(0)<v->hp&&v->getatk(1)>=u->hp&&v->type!="ninja"){
			printf("%03d:38 %s used a bomb and killed %s\n",game.t/60,string(*u).c_str(),string(*v).c_str());
			u=v=NULL;
		}
	}
}
void battle(int location,int p,bool defense){
	auto &u=C[location][p],&v=C[location][1-p];
	int tmp=max(0,v->hp);
	if(u->hp>0&&v->hp>0)
		if(!defense||u->type!="ninja"){
			v->hp-=u->getatk(defense);
			u->usesword(1);
			if(defense)
				printf("%03d:40 %s fought back against %s in city %d\n",game.t/60,string(*u).c_str(),string(*v).c_str(),location);
			else printf("%03d:40 %s attacked %s in city %d %s\n",game.t/60,string(*u).c_str(),string(*v).c_str(),location,u->info().c_str());
		}
	if(v->hp<=0&&u->hp>0){
		if(!arr[location])
			printf("%03d:40 %s was killed in city %d\n",game.t/60,string(*v).c_str(),location);
		win[location]=p;
		if(v->type=="lion")u->hp+=tmp;
		if(u->type=="wolf")
			for(int i=0;i<3;i++)
				if(!u->wea[i])u->wea[i]=v->wea[i];
	}
}
void Game::war(){
	memset(win,-1,sizeof(win));
	H[0].rew=H[0].hp/8;
	H[1].rew=H[1].hp/8;
	for(int i=1;i<=N;i++)
		if(C[i][0]&&C[i][1]){
			int p;
			if(flag[i]!=-1) p=flag[i];
			else p=!(i&1);
			battle(i,p,0);
			battle(i,1-p,1);
			if(C[i][p]->morale>0.8&&C[i][p]->hp>0)
				printf("%03d:40 %s yelled in city %d\n",game.t/60,string(*C[i][p]).c_str(),i);
			if(win[i]!=-1){
				int w=win[i];
				printf("%03d:40 %s earned %d elements for his headquarter\n",game.t/60,string(*C[i][w]).c_str(),chp[i]);
				H[w].hp+=chp[i];chp[i]=0;
				if(lastwin[i]==w&&flag[i]!=w){
					flag[i]=w;
					printf("%03d:40 %s flag raised in city %d\n",game.t/60,H[w].color.c_str(),i);
				}
				lastwin[i]=w;
			}
			for(int j=0;j<2;j++)
				if(C[i][j]->type=="dragon"){
					if(j==win[i])C[i][j]->morale+=0.2;
					else C[i][j]->morale-=0.2;
				}
			if(C[i][0]->hp>0&&C[i][1]->hp>0){
				for(int j=0;j<2;j++)
					if(C[i][j]->type=="lion")C[i][j]->loyalty-=K;
				lastwin[i]=-1;
			}
		}
	for(int i=1;i<=N;i++)
		for(int j=0;j<2;j++)
			if(C[i][j]&&C[i][j]->hp<=0)C[i][j]=NULL;
	for(int i=N;i&&H[0].rew;i--)
		if(win[i]==0){
			H[0].hp-=8;H[0].rew--;
			C[i][0]->hp+=8;
		}
	for(int i=1;i<=N&&H[1].rew;i++)
		if(win[i]==1){
			H[1].hp-=8;H[1].rew--;
			C[i][1]->hp+=8;
		}
}
void Game::turn(){
	int M=t%60;
	if(M==0) H[0].born(),H[1].born();
	if(M==5) escape();
	if(M==10) march();
	if(t==-1) return;
	if(M==20) for(int i=1;i<=N;i++) chp[i]+=10;
	if(M==30) gethp();
	if(M==35) shoot();
	if(M==38) usebomb();
	if(M==40) war();
	if(M==50)
		for(int i=0;i<2;i++)
			printf("%03d:50 %d elements in %s headquarter\n",game.t/60,H[i].hp,H[i].color.c_str());
	if(M==55){
		for(int i=0;i<=N+1;i++)
			if(C[i][0])print(*C[i][0]);
		for(int i=0;i<=N+1;i++)
			if(C[i][1])print(*C[i][1]);				 
	}
	t++;
}
void Game::print(warrior S){
	printf("%03d:55 %s has ",game.t/60,string(S).c_str());
	if(S.wea[2]){
		printf("arrow(%d)",S.wea[2]->atk);
		if(S.wea[1]||S.wea[0])putchar(',');
		else putchar('\n');
	}
	if(S.wea[1]){
		printf("bomb");
		if(S.wea[0])putchar(',');
		else putchar('\n');
	}
	if(S.wea[0])printf("sword(%d)\n",S.wea[0]->atk);
	if(!S.wea[0]&&!S.wea[1]&&!S.wea[2])puts("no weapon");
}

int main(){
	int tot;
	cin>>tot;
	for(int i=1;i<=tot;i++){
		cin>>M>>N>>R>>K>>T;
		H[0]=headquarter("red",M);
		H[1]=headquarter("blue",M);
		cin>>HP["dragon"]>>HP["ninja"]>>HP["iceman"]>>HP["lion"]>>HP["wolf"];
		cin>>ATK["dragon"]>>ATK["ninja"]>>ATK["iceman"]>>ATK["lion"]>>ATK["wolf"];
		game.reset(i);
		while(game.t<=T&&game.t!=-1)
			game.turn();
	}
	return 0;
}
