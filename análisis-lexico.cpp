//https://github.com/linyacool/lexical_syntax_analysis

#include <iostream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <stack>
#include <fstream>

using namespace std;

//numero de palabras key
const int num_of_keyword=22;
//numero de operadores unarios
const int num_of_MO=11;
//numero de operadores binarios
const int num_of_BO=12;
//numero de delimitadores
const int num_of_D=9;
//lista de palabras claves
const char keyword[50][12]={"break","case","char","continue","do","default","double",
"else","float","for","if","int","include","long","main","return","switch","typedef","void","unsigned","while","iostream"};
const char Monocular_Operator[20]={'+','-','*','/','!','%','~','&','|','^','='};   //Operador unario
const char Binocular_Operator[20][5]={"++","--","&&","||","<=","!=","==",">=","+=","-=","*=","/="}; //Operador binocular
const char Delimiter[20]={',','(',')','{','}',';','<','>','#'}; //Delimitador

FILE* file_source=NULL;
//entradas y salidas
ifstream input;
ofstream output;

/******************************************/
// Palabra clave 1
// Identificador 2
// Constante 3
// Operador 4
// Delimitador 5
/******************************************/

char f(char str[])        //Mapeado a un entero para representarlo
{
	if(strcmp(str,keyword[0])==0)
		return 'b';
	if(strcmp(str,keyword[1])==0)
		return 'c';
	if(strcmp(str,keyword[2])==0)   //char  a
		return 'a';
	if(strcmp(str,keyword[3])==0)   //continue   o
		return 'o';
	if(strcmp(str,keyword[4])==0)
		return 'd';
	if(strcmp(str,keyword[5])==0)	//default    n
		return 'n';
	if(strcmp(str,keyword[6])==0)	//double   z
		return 'z';
	if(strcmp(str,keyword[7])==0)
		return 'e';
	if(strcmp(str,keyword[8])==0)	//float  y
		return 'y';
	if(strcmp(str,keyword[9])==0)
		return 'f';
	if(strcmp(str,keyword[10])==0)
		return 'i';
	if(strcmp(str,keyword[11])==0)  //int    h
		return 'h';
	if(strcmp(str,keyword[12])==0)	//include  p
		return 'p';
	if(strcmp(str,keyword[13])==0)
		return 'l';
	if(strcmp(str,keyword[14])==0)
		return 'm';
	if(strcmp(str,keyword[15])==0)
		return 'r';
	if(strcmp(str,keyword[16])==0)
		return 's';
	if(strcmp(str,keyword[17])==0)
		return 't';
	if(strcmp(str,keyword[18])==0)
		return 'v';
	if(strcmp(str,keyword[19])==0)
		return 'u';
	if(strcmp(str,keyword[20])==0)
		return 'w';
	if(strcmp(str,keyword[21])==0)     //iostream   k
		return 'k';
}

//estados de inicio
char state[100];
int len_state;
char start;
//caracteres iniciales de las producciones
//son finales para el armado
char final[100];
int len_final;
bool es_final[150];

//Estructura de NFA con una lista de caracteres
struct NFA_set
{
	char set[100];
	int len=0;
};
char buffer[100];
int buffer_size=0;

//tabla de NFA de 100X100
//con las producciones
//filas son ch, columnas caracteres iniciales
NFA_set move_tabla[100][100];

char N_state[100];
int N_len_state;
char N_start;
char N_final[100];
int N_len_final;
bool N_es_final[100];

NFA_set new_set[100];
int num_new_set=0;
int dfa[150][150];

//reconocer enteros
bool IsInteger(char a)
{
	if(a>='0' && a<='9')
		return true;
	return false;
}

//reconocer letras
bool IsLetter(char a)
{
	if(a>='a' && a<='z')
		return true;
	if(a>='A' && a<='Z')
		return true;
	return false;
}

//reconocer palabras claves
bool IsKeyword(char a[])
{
	int len=strlen(a);
	for(int j=0;j<num_of_keyword;++j)
	{
		if(strlen(keyword[j])==len)
		{
			if(strcmp(keyword[j],a)==0)
				return true;
		}
	}
	return false;
}

//si operador unario
bool IsMO(char a)
{
	for(int i=0;i<num_of_MO;++i)
	{
		if(Monocular_Operator[i]==a)
			return true;
	}
	return false;
}

//operador binario
bool IsBO(char a[])
{
	for(int i=0;i<num_of_BO;++i)
	{
		if(strcmp(Binocular_Operator[i],a)==0)
			return true;
	}
	return false;
}

//Es un delimitador
bool IsDelimiter(char a)
{
	for(int i=0;i<num_of_D;++i)
	{
		if(Delimiter[i]==a)
			return true;
	}
	return false;
}

bool Is_In_state(char a)
{
	for(int i=0;i<len_state;++i)
	{
		if(a==state[i])
			return true;
	}
	return false;
}

bool Is_In_final(char a)
{
	for(int i=0;i<len_final;++i)
	{
		if(a==final[i])
			return true;
	}
	return false;
}

bool Is_in_set(char a,NFA_set temp)
{
	for(int i=0;i<temp.len;++i)
	{
		if(a==temp.set[i])
			return true;
	}
	return false;
}

//crea el NFA por tabla
void createNFA()
{

	int N;
	bool flag=true;   //Flag solo para el primero
	char ch;    //Se usa para leer el lado izquierdo de la gramática
	char nouse;   //Absorbe caracteres basura
	char str[10];    //Se usa para leer gramática a la derecha
	//leemos la gramatica
	input.open("gramatica.txt");
	//numero de producciones
	input>>N;
	while(N--)
	{
		//lee una cadena de la forma A->cualquier cosa
		input>>ch>>nouse>>nouse>>str;
		//si es el primero crea la produccion
		if(flag)
		{
			start=ch;
			flag=false;
		}
		//si no esta en state lo agrega
		if(!Is_In_state(ch))
		{
			//vector de estados
			state[len_state++]=ch;
		}
		//si no esta en final lo agrega
		if(!Is_In_final(str[0]))
		{
			final[len_final++]=str[0];
		}
		//si la cadena no es vacia
		if(strlen(str)>1)
		{
			move_tabla[ch][str[0]].set[move_tabla[ch][str[0]].len++]=str[1];
		}//si es una cadena de produccion vacia
		else
		{
			move_tabla[ch][str[0]].set[move_tabla[ch][str[0]].len++]='Y';  //Estado final
		}
	}
}

//Imprime el NFA
void showNFA()
{
	for(int i=0;i<100;++i)
	{
		for(int j=0;j<100;++j)
		{
			for(int k=0;k<100;++k)
			{
				if(move_tabla[i][j].set[k]!='#')
					cout<<char(i)<<" "<<char(j)<<" "<<k<<" "<<move_tabla[i][j].set[k]<<endl;
			}
		}
	}
}

int Is_in(NFA_set temp)   //Si hay algún duplicado con el nuevo conjunto existente, se devolverá el número duplicado
{
	bool flag[100];
	bool flag1;
	for(int i=0;i<temp.len;++i)
	{
		flag[i]=false;
	}
	for(int i=0;i<num_new_set;++i)
	{
		for(int k=0;k<temp.len;++k)
		{
			for(int j=0;j<new_set[i].len;++j)
			{
				if(temp.set[k]==new_set[i].set[j])
				{
					flag[k]=true;
				}
			}
		}
		flag1=true;
		for(int m=0;m<temp.len;++m)
		{
			if(flag[m]==false)
			{
				flag1=false;
				break;
			}
		}
		if(flag1==true)
			return i;
		for(int m=0;m<temp.len;++m)
		{
			flag[m]=false;
		}
	}
	return -1;
}

//retorna la clausura de un elemento
void get_closure(NFA_set &temp)    //Obtenga un subconjunto completo
{
	for(int i=0;i<temp.len;++i)
	{
		for(int j=0;j<move_tabla[temp.set[i]]['@'].len;++j)
		{
			if(!Is_in_set(move_tabla[temp.set[i]]['@'].set[j],temp))
			{
				temp.set[temp.len++]=move_tabla[temp.set[i]]['@'].set[j];
			}
		}
	}
}

bool Is_contained_Y(NFA_set temp)   //Determinar si es el estado final
{
	for(int i=0;i<temp.len;++i)
	{
		if(temp.set[i]=='Y')
			return true;
	}
	return false;
}
void NFA_to_DFA()
{
	//Crea las variables auxiliares para crear el automata
	num_new_set=0;
	NFA_set work_set;
	NFA_set worked_set;
	//inicia los calculos
	work_set.set[work_set.len++]=start;
	worked_set.len=0;

	stack<NFA_set> s;
	get_closure(work_set);
	s.push(work_set);
	new_set[num_new_set++]=work_set;

	//rellena el DFA de valores vacios
	for(int i=0;i<150;++i)
	{
		for(int j=0;j<150;++j)
		{
			dfa[i][j]='-1';
		}
	}

	for(int i=0;i<150;++i)
		es_final[i]=false;
	if(Is_contained_Y(work_set))
		es_final[num_new_set-1]=true;
	while(!s.empty())
	{
		work_set=s.top();
		s.pop();
		for(int i=0;i<len_final;++i)
		{
			for(int j=0;j<work_set.len;++j)
			{
				for(int k=0;k<move_tabla[work_set.set[j]][final[i]].len;++k)
				{
					if(move_tabla[work_set.set[j]][final[i]].set[k]!='#' && move_tabla[work_set.set[j]][final[i]].set[k]!='Y' && !Is_in_set(move_tabla[work_set.set[j]][final[i]].set[k],worked_set))
					{
						worked_set.set[worked_set.len++]=move_tabla[work_set.set[j]][final[i]].set[k];
					}
					if(move_tabla[work_set.set[j]][final[i]].set[k]=='Y' && !Is_in_set(move_tabla[work_set.set[j]][final[i]].set[k],worked_set))
					{
						worked_set.set[worked_set.len++]='Y';    //Use Y para el estado final
					}
				}
			}
			get_closure(worked_set);
			if(worked_set.len>0 && Is_in(worked_set)==-1)
			{
				dfa[num_new_set-1][final[i]]=num_new_set;
				s.push(worked_set);
				new_set[num_new_set++]=worked_set;
				if(Is_contained_Y(worked_set))
				{
					es_final[num_new_set-1]=true;
				}
			}
			if(worked_set.len>0 && Is_in(worked_set)>-1 && final[i]!='@')
			{
				dfa[Is_in(work_set)][final[i]]=Is_in(worked_set);
			}
			worked_set.len=0;
		}
	}
}

bool DFA(char str[])
{
	char now_state=0;
	for(int i=0;i<strlen(str);++i )
	{
		now_state=dfa[now_state][str[i]];
		if(now_state==-1)
			return false;
	}
	if(es_final[now_state]==true)
		return true;
	return false;
}

//comienza el reconocimiento
void scan()
{
	char str[100];
	char ch;
	int i,j;
	int point;
	int flag;

	ch=fgetc(file_source);
	bool finish=false;
	while(!finish)
	{
	 	flag=-1;
		point=0;
		//Si es un numero le asigna flag 1
		if(IsInteger(ch))     //Un ch más
		{
			flag=1;
			str[point++]=ch;
			//luego toma el proximo caracter
			ch=fgetc(file_source);
			while(IsLetter(ch) || IsInteger(ch) || ch=='.' || ch=='+' || ch=='-')
			{
				flag=1;
				str[point++]=ch;
				ch=fgetc(file_source);
			}
			//coloca el fin de la cadena
			str[point]='\0';
		}
		//Busca el valor en el DFA
		if(flag==1)
		{
			if(DFA(str))
			{
				cout<<str<<" constante"<<endl;
				output<<3;
			}
			else
			{
				cout<<str<<" "<<"Error no constante"<<endl;
			}
			point=0;
			flag=-1;
		}

		//Si es una letra asigna flag 2
		if(IsLetter(ch))
		{
			flag=2;
			str[point++]=ch;
			ch=fgetc(file_source);
			while(IsLetter(ch) || IsInteger(ch))
			{
				flag=2;
				str[point++]=ch;
				ch=fgetc(file_source);
			}
			str[point]='\0';
		}
		//verifica si la palabra es un keyword
		if(flag==2)
		{
			if(IsKeyword(str))
			{
				cout<<str<<" "<<"Palabra clave"<<endl;
				output<<f(str);
			}
			else
			{
				//Lo busca en el DFA
				if(DFA(str))
				{
					cout<<str<<" "<<"Identificador"<<endl;
					output<<2;
				}
				else
				{
					cout<<str<<" "<<"Error, no es un identificador"<<endl;
				}
			}
			point=0;
			flag=-1;
		}
		//si es un delimitador
		if(IsDelimiter(ch))
		{
			cout<<ch<<" "<<"Delimitador"<<endl;
			if(ch=='#')
				output<<'*';
			else
				output<<ch;
			if((ch=fgetc(file_source))==EOF)
			{
				finish=true;
				break;
			}
		}

		//si es operador unario
		if(IsMO(ch))
		{
			str[point++]=ch;
			if((ch=fgetc(file_source))==EOF)
			{
				finish=true;
			}
			str[point++]=ch;
			str[point]='\0';
			if(finish==false && IsBO(str))
			{
				cout<<str<<" "<<"Operador binocular"<<endl;
				//output<<4;
				ch=fgetc(file_source);
			}
			else
			{
				cout<<str[0]<<" "<<"Operador unario"<<endl;
				output<<str[0];
			}
			point=0;
		}

		//si es vacio o salto de linea u otro
		if(ch==' ' || ch=='\n' || ch=='\t')
		{
			if((ch=fgetc(file_source))==EOF)
			{
				finish=true;
				break;
			}
			continue;
		}
	}
	output<<'#';
}

//rellena la tabla de 100X100 con valores de #
void init()
{
	len_final=0;
	len_state=0;
	for(int i=0;i<100;++i)
	{
		for(int j=0;j<100;++j)
			for(int k=0;k<100;++k)
				move_tabla[i][j].set[k]='#';
	}
}

void show()
{
	for(int i=0;i<num_new_set;++i)
	{
		cout<<"Conjunto "<<i<<endl;
		for(int j=0;j<new_set[i].len;++j)
		{

			cout<<new_set[i].set[j]<<" ";
		}
		cout<<endl;
	}
}
int main()
{
	//Inicializa la tabla con valores #
	init();
	len_final=0;
	len_state=0;
	//Creacion del Automata
	createNFA();
	NFA_to_DFA();
	show();
	file_source=fopen("entrada.txt","r+");
	output.open("output.txt");
	//Revision si la entrada pertenece a la gramatica
	scan();
	fclose(file_source);
	output.close();
	return 0;
}