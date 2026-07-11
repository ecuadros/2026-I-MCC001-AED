/**
 * @file BTreeDemo.cpp
 * @brief Archivo de prueba del árbol B.
 *
 * En este demo probé las funcionalidades nuevas que fui implementando
 * en el árbol B: inserción, impresión, ForEach global con iteradores,
 * iterador forward, iterador backward, FirstThat y Search.
 */

//#include <iostream.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include "BTree.h"

//const char * keys="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const char * keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
const char * keys2 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char * keys3 = "DYZakHIUwxVJ203ejOP9Qc8AdtuEop1XvTRghSNbW567BfiCqrs4FGMyzKLlmn";

const TI BTreeSize = 3;

/**
 * @brief Ejecuta una demostración básica del contenedor BTree.
 *
 * Este método inserta datos de prueba y luego ejecuta los recorridos
 * y algoritmos que se pidieron en la tarea para validar que el árbol
 * ya trabaja con iteradores y con los helpers globales.
 */
void BTreeDemo()
{
       TI i;
       BTree<BTreeTraits<char, TI>> bt(BTreeSize);
       for (i = 0; keys1[i]; i++)
       {
               //cout<<"Inserting "<<keys1[i]<<endl;
               bt.Insert(keys1[i], i*i);
               //bt.Print(cout);
       }
       // Imprecion original
       bt.Print(cout);

       cout << "FOREACH GLOBAL CON ITERADORES" << endl;
       bt.ForEach([](auto &node){
                 cout << node.key << "->" << node.ObjID << endl;
       });

       cout << "ITERADOR FORWARD" << endl;
       for(auto it = bt.begin(); it != bt.end(); ++it)
       {
               cout << it->key << "->" << it->ObjID << endl;
       }

       cout << "ITERADOR BACKWARD" << endl;
       for(auto it = bt.rbegin(); it != bt.rend(); ++it)
       {
               cout << it->key << "->" << it->ObjID << endl;
       }

       cout << "FIRSTHAT" << endl;
       auto pNode = bt.FirstThat([](auto &node){
                 return node.key >= 'M';
       });

       if(pNode)
        {
                 cout << "FirstThat found: " << pNode->key << "->" << pNode->ObjID << endl;
        }
        else
        {
                 cout << "FirstThat not found" << endl;
        }

       cout << "SEARCH" << endl;
        auto value = bt.Search('M');
        if(value != -1)
        {
                 cout << "Search found: " << value << endl;
        }
        else
        {
                 cout << "Search not found" << endl;
        }


       /*for (i = 0; keys2[i]; i++)
       {
               cout << "Searching " << keys2[i] << " ";
               long ObjID = bt.Search(keys2[i]);
               if( ObjID != -1 )
                       cout << "Achei " << keys2[i] << " ID = " << ObjID << endl;
               else
                       cout <<"Nao achei!" << keys2[i] << endl;
       }*/
       /*cout.flush();

       for (i = 0; keys3[i]; i++)
       {
               cout << "Removing " << keys3[i] << " ";
               if( bt.Remove(keys3[i], -1) )
                       cout << keys3[i] << " removido !" << endl;
               else
                       cout <<"Nao achei!" << keys3[i] << endl;
               bt.Print(cout);
       }
       bt.Print(cout);
       cout.flush();*/
       return;
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
