#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include "../types.h"
#include "../foreach.h"

using namespace std;

/**
 * @file STLTextAnalyzerDemo.cpp
 * @brief Demo de analisis de texto usando clases STL.
 *
 * En este archivo se muestra el uso conjunto de vector, map, set,
 * stringstream y algoritmos de la STL para procesar un texto, contar
 * palabras, ordenarlas por frecuencia y filtrar por prefijo.
 */

/**
 * @brief Trait base para parametrizar el analizador de texto.
 *
 * @tparam TextT Tipo de texto o palabra.
 * @tparam SymbolT Tipo de simbolo individual.
 * @tparam CountT Tipo numerico para conteos.
 */
template <typename TextT, typename SymbolT, typename CountT>
struct BaseTextAnalyzerTrait {
    using text_type   = TextT;
    using symbol_type = SymbolT;
    using count_type  = CountT;
};

/**
 * @brief Trait concreto del demo para trabajar con string y char.
 */
using STLTextAnalyzerTrait = BaseTextAnalyzerTrait<TS, TC, TI>;

/**
 * @brief Normaliza una palabra para su analisis.
 *
 * Elimina caracteres que no sean letras o digitos y convierte
 * todo a minuscula para poder contar palabras equivalentes
 * como una sola entrada.
 *
 * @param word Palabra original leida del texto.
 * @return Palabra limpia y normalizada.
 */
template <typename Traits>
static typename Traits::text_type NormalizeWord(typename Traits::text_type word) {
    using text_type   = typename Traits::text_type;
    using symbol_type = typename Traits::symbol_type;

    text_type clean;

    for(symbol_type ch : word) {
        unsigned char uch = static_cast<unsigned char>(ch);
        if(isalpha(uch) || isdigit(uch))
            clean.push_back(static_cast<symbol_type>(tolower(uch)));
    }

    return clean;
}

/**
 * @brief Analizador de texto parametrizado con Traits.
 *
 * Reutiliza varias clases STL para separar palabras, contar
 * frecuencias, eliminar duplicados y ordenar resultados.
 */
template <typename Traits>
class STLTextAnalyzer {
private:
    using text_type   = typename Traits::text_type;
    using symbol_type = typename Traits::symbol_type;
    using count_type  = typename Traits::count_type;

public:
    /**
     * @brief Ejecuta la demo de analizador de texto con STL.
     *
     * La demo realiza las siguientes tareas:
     * - almacena varias lineas de texto en un vector
     * - separa las palabras usando stringstream
     * - normaliza cada palabra
     * - cuenta frecuencias con map
     * - obtiene palabras unicas con set
     * - genera un ranking ordenado con sort
     * - filtra palabras por prefijo
     * - detecta la palabra mas larga del texto
     */
    static void Demo() {
        vector<text_type> lines = {
            "La STL permite reutilizar estructuras de datos y algoritmos.",
            "Con vector, map y set podemos analizar texto con pocas lineas.",
            "La STL tambien facilita ordenamiento, conteo y eliminacion de duplicados.",
            "Aprender STL ayuda a programar mejor y mas rapido."
        };

        vector<text_type> words;
        map<text_type, count_type> frequency;
        set<text_type> uniqueWords;

        /// Se recorre cada linea y se separa en palabras individuales.
        ::ForEach(lines, [&](const text_type& line) {
            stringstream ss(line);
            text_type word;

            while(ss >> word) {
                word = NormalizeWord<Traits>(word);
                if(word.empty())
                    continue;

                words.push_back(word);
                ++frequency[word];
                uniqueWords.insert(word);
            }
        });

        /// Se construye un ranking a partir del map para poder ordenarlo.
        vector<pair<text_type, count_type>> ranking(frequency.begin(), frequency.end());
        sort(ranking.begin(), ranking.end(),
             [](const auto& left, const auto& right) {
                if(left.second != right.second)
                    return left.second > right.second;
                return left.first < right.first;
             });

        /// Prefijo de ejemplo para mostrar una consulta simple.
        const text_type prefix = text_type{symbol_type('s'), symbol_type('t')};

        cout << "\n===== DEMO 1: ANALIZADOR DE TEXTO CON STL =====\n\n";

        cout << "Texto original:\n";
        ::ForEach(lines, [](const auto& line) {
            cout << "- " << line << '\n';
        });

        cout << "\nTotal de palabras procesadas: " << words.size() << '\n';
        cout << "Cantidad de palabras unicas : " << uniqueWords.size() << '\n';

        cout << "\nPalabras unicas ordenadas:\n";
        ::ForEach(uniqueWords, [](const auto& word) {
            cout << word << ' ';
        });
        cout << "\n";

        cout << "\nFrecuencias:\n";
        ::ForEach(frequency, [](const auto& entry) {
            cout << setw(15) << left << entry.first
                 << " -> " << entry.second << '\n';
        });

        cout << "\nTop 5 palabras mas frecuentes:\n";
        for(size_t i = 0; i < ranking.size() && i < 5; ++i) {
            cout << i + 1 << ". "
                 << ranking[i].first
                 << " -> " << ranking[i].second << '\n';
        }

        cout << "\nBusqueda por prefijo \"" << prefix << "\":\n";
        ::ForEach(uniqueWords, [&](const auto& word) {
            if(word.starts_with(prefix))
                cout << "- " << word << '\n';
        });

        /// Se busca la palabra mas larga comparando tamanos.
        auto longest = max_element(words.begin(), words.end(),
            [](const auto& left, const auto& right) {
                return left.size() < right.size();
            });

        if(longest != words.end())
            cout << "\nPalabra mas larga detectada: " << *longest << '\n';

        cout << "\n===== FIN DEMO 1 =====\n";
    }
};

/**
 * @brief Ejecuta la demo concreta usando el trait del proyecto.
 *
 * Esta funcion conserva una interfaz simple para el main, pero
 * internamente usa una clase plantilla parametrizada con Traits.
 */
void STLTextAnalyzerDemo() {
    STLTextAnalyzer<STLTextAnalyzerTrait>::Demo();
}
