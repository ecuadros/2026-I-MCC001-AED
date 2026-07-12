//#include <iostream.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include "BTree.h"
#include "../lists.h"
//const char * keys="CDAMPIWNBKEHOLJYQZFXVRTSGU";
using TCH = char;
using TI = int;
const TCH * keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
const TCH * keys2 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const TCH * keys3 = "DYZakHIUwxVJ203ejOP9Qc8AdtuEop1XvTRghSNbW567BfiCqrs4FGMyzKLlmn";

const TI BTreeSize = 3;
//int main(int argc, char * argv[], char * envp[])
void DemoBTree()
{
       TI i;
       BTree <TCH> bt (BTreeSize);
       for (i = 0; keys1[i]; i++)
       {
               bt.Insert(keys1[i], i*i);
       }
       std::cout << "=== Estado inicial del BTree ===" << std::endl;
       bt.Print(cout);
       
       std::cout << "\n=== Prueba de Busqueda ===" << std::endl;
       for (i = 0; keys2[i] && i < 10; i++)
       {
               long ObjID = bt.Search(keys2[i]);
               if( ObjID != -1 )
                       cout << "Encontrado: " << keys2[i] << " -> ObjID = " << ObjID << endl;
               else
                       cout << "No encontrado: " << keys2[i] << endl;
       }
       std::cout << "\n=== Prueba de FirstThat ===" << std::endl;       
       //Solo es una prueba ::Search es mas optimo que FirstThat, pero sirve para probar el iterador
       auto searchkey = [](const auto& node, TI searchKey) {
              return node.key == searchKey;
       };
       
       for (i = 0; keys2[i] && i < 10; i++){
            auto it = bt.FirstThat(searchkey, keys2[i]);
            if(it){
                cout << "Encontrado: "<< keys2[i] << " -> ObjID = " << *it << endl;
            }else{

                cout << "No encontrado: " << keys2[i] << endl;
            }
       }

       cout<<std::endl;

       std::cout << "\n=== Prueba de Iterador (begin/end) ===" << std::endl;
       for (auto node : bt)
       {
               cout << node.key << "(" << node.ObjID << ") ";
       }
       cout << endl;
       std::cout << "\n=== Prueba de Iterador Inverso (rbegin/rend) ===" << std::endl;
        for (auto node = bt.rbegin(); node != bt.rend(); ++node){
                        cout << node->key << "(" << node->ObjID << ") ";
        }
        cout << endl;
        
        std::cout<< "\n=== Prueba ForEach ===" << std::endl;
        auto printNode = [](const auto& node){
            cout << node << " ";
        };
        bt.ForEach(printNode);
        cout << endl;
}

/*const char * keys="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const char * keys2="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const int BTreeSize = 3;
main (int argc, char * argv)
{
       //__int64 li;
       BTree <__int64> bt (BTreeSize);
       for (register int i = 0; i < 1000000; i++)
       {
               //cout<<"Inserting "<<keys[i]<<endl;
               bt.Insert(i, i-1);
               //bt.Print(cout);
       }

       for (i = 0; i < 1000; i++)
       {
               __int64 key = 975000+(::rand()%50000);
               //cout << "Searching " << (long)key << " ";
               long ObjID = bt.Search(key);
               if( ObjID != -1 )
                       cout << "Achei " << (long)key << " ID = " << ObjID << endl;
               else
                       cout <<"  Nao achei!" << (long)key << endl;
       }
       cout.flush();

       return 1;
}*/



/*const int BTreeSize = 3;
main (int argc, char * argv)
{
       int result, i;
       BTree <LONGLONG> bt(BTreeSize);
       result = bt.Create ("ernesto3-string-btree-start.dat",ios::in|ios::out);
       if (!result) { cout<<"Please delete testbt.dat"<<endl;return 0; }
       srand( (unsigned)time( NULL ) );
       LARGE_INTEGER key;
       for (i = 0; i < 1000000; i++)
       {
               //cout<<"Inserting "<<keys[i]<<endl;
               char strTmp[50];
               key.LowPart = rand();
               key.HighPart = rand();
               std::string str(strTmp);
               result = bt.Insert(key.QuadPart, i);
               //bt.Print(cout);
               if( i % 100000 == 0 )
               {       cout << i << endl; cout.flush();        }
       }
       //cout << "Searching D " << bt.Search();
       //bt.Search(1,1);
       cout.flush();
       return 1;
}*/
