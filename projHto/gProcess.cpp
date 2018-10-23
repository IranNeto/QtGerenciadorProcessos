#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>
#include <cstdlib>
#include <curses.h>
#include <math.h> 
//#include <features.h>
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <sched.h> 
#include <termio.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

using namespace std;


//the following are UBUNTU/LINUX ONLY terminal color codes.
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

//listar processos ordenados pelo resident set size, the non-swapped physical memory that
string listPCommand = "ps -aeo user,comm,pid,psr,pcpu,pri,ni,cputime --sort -rss | head -30";

//mudar cpu processo
void task_set(int pid,int cpu) {
        //int result;
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(cpu, &mask);
    if((sched_setaffinity(pid, sizeof(mask), &mask)) == 0){
        cout<<"Done\n";
        sleep(2);
        system("clear");
        //sleep(1);
    }else{
        cout<<"Error\n";
        //sleep(1);
    }
        /*char comand[40];
        sprintf(comand, "taskset -cp %d %d", cpu, pid);
        system(comand);*/
}


int main(){
    int escolha,newCpu,newPrio;
    pid_t pidShot;
    string palavraChave;
    bool error=0;
    do{
        system("clear");
        system(listPCommand.c_str());
        if(error){
            cout<<"Operação inválida!\n";
            error!=error;
        }
        cout << BOLDRED << "---------\n1-Pausar | 2-Continuar | 3-Parar | 4-Mudar Psr | 5-Mudar Pri | 6-Tree | 7 - Filtro | 8- %CPU \n 0-Exit\n";
        cout<<"Digite sua escolha: " << RESET;
        cin>>escolha;
        while(escolha>8 || escolha<0){
            cout<<"Operação inválida, digite novamente.\n";
            cin>>escolha;
        }
        
        if(escolha < 7 && escolha != 0){
            cout<<endl<<"Digite o PID do processo: ";
            cin>>pidShot;
        }
        
        switch(escolha){
            case 1:
                if(!(kill(pidShot,SIGSTOP))){
                    cout<<"Pausei! \n";
                }else{
                    cout<<"Operacao invalida\n";
                }
                sleep(1);
                break;
            case 2:
                if(!(kill(pidShot,SIGCONT))){
                    cout<<"Continuei! \n";
                }else{
                    cout<<"Operacao invalida\n";
                }
                sleep(1);
                break;
            case 3:
                if(!(kill(pidShot,SIGKILL))){
                    cout<<"Matei! \n";
                }else{
                    cout<<"Operacao invalida\n";
                }
                sleep(1);
                break;
            case 4: 
                cout<<"Digite a nova CPU: ";
                cin>>newCpu;
                //testar se é valida "nproc --all"
                task_set(pidShot,newCpu);
                break;
            case 5:
                cout<<"Digite o novo valor Nice: ";
                cin>>newPrio;
                if(setpriority(PRIO_PROCESS, pidShot, newPrio) == -1){
                    cout<<"Erro ao setar prioridade, super user?\n";
                }
                sleep(1);
                break;
            
            case 6:
                char treeCommand[40];
                sprintf(treeCommand, "pstree %d -u", pidShot);
                system((treeCommand));
                system("read -p \"Pressione enter para sair\" saindo");
                break;
                
            case 7:
                cout<< "Digite uma palavra-chave para a busca: ";
                cin >> palavraChave;
                cout << endl << "Resultado: " << endl;
                system((listPCommand + " | grep " + palavraChave).c_str());
                //system("pause");
                system("read -p \"Pressione enter para sair\" saindo");
                break;
            
            case 8: 
            { //precisa dessas chaves para especificação de escopo das variáveis desse caso
                const int nCores = 4; //Deixar nCores estático enquanto não descobrir um jeito de indentificar quantos cores existem
                string line;
                string lines[30];
                string tokenToSearch[nCores];
                
                string splitChar = " ";
                string token;
                string item[10];

                int total[nCores] = {0,0,0,0};
                int idle[nCores] = {0,0,0,0};
                int prevIdle[nCores] = {0,0,0,0};
                int prevTotal[nCores] = {0,0,0,0};
                int diffTotal[nCores] = {0,0,0,0};
                int diffIdle[nCores] = {0,0,0,0};
                int nLine = 0;
                float percent;
               
                system("sar -P 1 1 > cpuUsage");
                ifstream infile("cpuUsage");

                while(getline(infile, line)){
                    istringstream iss(line);
                    //passa o stream para um vetor de strings
                    if(!(iss >> lines[nLine])){iss.clear();break;}
                    
                    //stream para pegar as informações da linha
                    stringstream rightLine(line);
                    
                    //checa o cabeçalho da linha
                    token = lines[nLine].substr(0, lines[nLine].find(splitChar));

                    cout << endl << token << endl;

                    system("read -p \"Pressione enter para voltar\" saindo");
                    infile.clear();
                    
                    break;
                }
            }
            default:
                //error!=error;
                break;
        }
    }while(escolha!=0);
    cout << "Saindo..." << endl;
    exit(1);
}
        
