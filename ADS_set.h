//Header-Guards sind Teil einer Header Datei
#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>

//Definition der Template Class (Vorlagenklasse) ADS_set
template <typename Key, size_t N = 7> //Parameter Key ist der Datentyp der Elemente die gespeichert werden sollen, N ist Größe der Tabelle
class ADS_set {
public:

    class Iterator;
    using value_type = Key;						//value_type als Alias für Key
    using key_type = Key; 						//key_type als Alias für Key
    using reference = key_type&;				//Referenz (Ampersand&) als Alias für eine Referenz auf value_type /Ermöglicht direkten Zugriff auf Wert einer Variable ohne Kopie des Werts zu erstellen
    using const_reference = const key_type&;  	//const Referenz kann Wert nicht ändern sondern nur lesen (Schreibschutz)
    using size_type = size_t;					//size_type als Alias für Größen und Indizes
    using difference_type = std::ptrdiff_t;		//difference_type als Alias, ptrdiff_t wird genutzt um den Unterschied zwischen zwei Zeigern oder Iteratoren im Speicher darzustellen 
    using const_iterator = Iterator;
    using iterator = Iterator;
    using key_equal = std::equal_to<key_type>; // Hashing /Gleichheitsprüfung zwischen zwei Schlüsseln
    using hasher = std::hash<key_type>;        // Hashing /Berechnung des Hash Werts von Schlüsseln
	
	enum class Mode { FREE, USED, FREEAGAIN, END };	//Aufzählung Mode die Zustand des Elements beschreibt /freeagain nicht nötig eigentlich


private:
    struct Element {								//Struktur welche das Element (Bucket) darstellt
        key_type key;								//der zu speichernde wert vom Typ key_type
        Mode mode {Mode::FREE};						//Der Zustand, zu beginn free
		Element* next {nullptr};					//Pointer auf das nächste Element in der verketteten Liste, zu Beginn Nullpointer
        ~Element() {								//Dekonstruktor, Ressourcen werden wieder freigegeben
            delete next;							//löscht Adresse zum nächten Wert
        }
    };

    // Private Member Funktionen
    Element** table {nullptr};	//tabelle mit {nullptr} initialisiert
    size_type table_size {0};	//wie groß tabelle mit initialisierung 0
    size_type current_size {0};	//wie viele werte gespeichert

    float max_lf {0.7};			//maximaler Loadfactor, hier gerne experimentieren

    // Private Member Funktionen
    void add(const key_type& key);	//funktion zum einfügen
    size_type locate(Element* current, const key_type& key) const; //finden des Elements

    size_type h(const key_type& key) const { return hasher{}(key) % table_size; }  //hashfunktion, bekommt als parameter einen Schlüssel übergeben, müssen Position liefern die nach Hashfunktion in tabelle vorgesehen ist, standardfunktion hasher mit Übergabe eines Key 
    
	void rehash(size_type n);	//Für Anzahl werte n muss platz sein, Tabellengröße wird mit Loadfactor berechnet und angepasst
    void reserve(size_type n);	//Werte wird umgespeichert auf neue Tabellengröße

	//Versuch Performance zu erhöhen indem Tabellengröße Prim ist -> verminderung Chance von Kollisionen
    size_type get_next_prime(size_type n); 	//nimmt n und sucht die nächste Primzahl mit is_prime Funktion
    bool is_prime(size_type n); 			//checkt ob n+1 von get_next_prime ist prim

public:
//___________________________KONSTRUKTOREN___________________________//

    ADS_set() { rehash(N); } 														//PH1 Default Konstruktuor, übergibt an rehash, wo eine leere Instanz der Tabelle erstellt wird
    ADS_set(std::initializer_list<key_type> ilist) : ADS_set() { insert(ilist); } // PH1 initializer list Konstruktor,nimmt die Initialisierungsliste von Schlüsselwerten entgegen und fügt sie in Menge
    template<typename InputIt> ADS_set(InputIt first, InputIt last) : ADS_set () { insert(first,last); } // PH1 Range Konstruktor fügt Werte aus Range first last ein
    ADS_set(const ADS_set& other); 												//Kopierkonstruktor, Erzeugt einen Container, der dieselben Elemente wie der Container other erhält (genau so aussieht wie vorlage)
    ~ADS_set();																 //Destruktor, Die Destruktoren der gespeicherten Elemente werden aufgerufen und der verwendete Speicherplatz wieder freigegeben
    ADS_set &operator=(const ADS_set& other);	//Kopierzuweisungsopertator, der Inhalt des Containers wird durch den Inhalt von other ersetzt, Rückgabewert: Referenz auf *this
    ADS_set& operator=(std::initializer_list<key_type> ilist); //Initializer-List-Zuweisungsoperator. Der Inhalt des Containers wird durch die Elemente aus ilist ersetzt. Die Elemente werden in der in ilist vorgegebenen Reihenfolge eingefügt. Rückgabewert: Referenz auf *this

//___________________________METHODEN___________________________//


    size_type size() const { return current_size; } // PH1 die current size (gibt Anzahl der in ADS_set Instanz gespeicherte Werte zurück)
    bool empty() const { return current_size == 0; } // PH1 prüft ob ADS_set Instanz leer ist wenn current size 0, dann table empty (true zurückgegeben)

    void insert(std::initializer_list<key_type> ilist) { insert(ilist.begin(), ilist.end()); } // PH1 Fügt Elemente aus ilist in vorgegebener Reihenfolge ein. Diese Methode ermöglicht Einfügen von Liste mit Schlüsselwerten (ilist), ruft die insert-Funktion auf mit den Iteratoren ilist.begin(), end() um aus der Liste in Instanz einzufügen
    std::pair<iterator, bool> insert(const key_type& key); //Einzelwert Insert.Fügt key ein, Rückgabewert std::pair bestehend aus einem Iterator und einem Bool. Iterator zeigt auf das eingefügte Element falls eins eingefügt wurde, bzw. auf das bereits vorhandene mit demselben key falls keins eingefügt wurde. bool true falls eins eingefügt, false wenn nicht 
    
	iterator find(const key_type& key) const; //Rückgabewert ist ein Iterator auf das Element mit Schlüssel key oder End-Iterator end() falls key nicht vorghanden ist

	template<typename InputIt> void insert(InputIt first, InputIt last); //Fügt Elemente aus Bereich first,last in vorgegebener Reihenfolge beginnend mit first ein.Range-Konstruktor fügt Liste zwischen Iteratoren first und last ein

   	void clear(); 																			//Entfernt alle Elemente aus dem Container
  	size_type erase(const key_type &key); 													//Entfernt das Element key

    size_type count(const key_type& key) const { return locate(table[h(key)], key); } // PH1 Suchen ob bestimmter Schlüsselwert in Tabelle gespeichert ist, wenn wert gefunden 1  sonst 0. Zum Finden wird Hilfsfunktion locate genutzt
    
    void swap(ADS_set& other); //Vertauscht die Elemente des Containers mit den Elementen des Containers other

    const_iterator begin() const { //Liefert einen Iterator auf das erste Element im Container bzw EndIterator end() falls Container leer ist
        size_type i = 0;
        while (i < table_size && !table[i]) { ++i; }
        return (i < table_size) ? const_iterator(table, table[i], i, table_size) : end();
    }	

    const_iterator end() const { return const_iterator(); } //Liefert einen Iterator auf das (virtuelle) Element nach dem letzten Element im Container -> Rückgabewert End Iterator

    void dump(std::ostream& o = std::cerr) const; //Methode gibt aktuellen Zustand der Hashtabelle mit Ausgabestrom o aus. Es wird std:cerr verwendet wenn kein Ausgabestrom angegeben wird

   friend bool operator==(const ADS_set& lhs, const ADS_set& rhs) {	 //Vergleichsoperatoren, sind 2 ADS Sets gleich und besitzen gleiche Elemente?			
	if (lhs.current_size != rhs.current_size) { //Sets müssen gleich groß sein wenn sie gleich sein sollen
	return false; //Wir finden Elemente welche nicht im anderen Set sind
	} //Ansonsten true

	for (const auto &k : rhs) { 
            if (!lhs.count(k)) {
                return false;
            }
        }
        return true;
    }
    friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs) { return !(lhs == rhs); }

	
    //_________Iteratorklasse___________//
    class Iterator {
        Element** table;
        Element* element;
        size_type i, table_size;
        void skip();

    public:
        using value_type = Key;
        using difference_type = std::ptrdiff_t;
        using reference = const value_type &;
        using pointer = const value_type *;
        using iterator_category = std::forward_iterator_tag;

        explicit Iterator(Element** table = nullptr, Element* element = nullptr, size_type i = 0, size_type size = 0)
            : table{table}, element{element}, i{i}, table_size{size} {}
        reference operator*() const { return element->key; }
        pointer operator->() const { return &element->key; }
        Iterator& operator++();
        Iterator operator++(int) { auto rc {*this}; ++*this; return rc; }
        friend bool operator==(const Iterator &lhs, const Iterator &rhs) { return lhs.element == rhs.element; }
        friend bool operator!=(const Iterator &lhs, const Iterator &rhs) { return !(lhs == rhs); }
    };
};



template <typename Key, size_t N> //durch Tabelle iterieren und Buckets löschen DESTRUKTOR
ADS_set<Key, N>::~ADS_set() {
    for (size_type i = 0; i < table_size; ++i) {
        delete table[i];
    }
    delete[] table;
}

//_________Hilfsfunktion_add___________//
template <typename Key, size_t N> //Definition der Funktion, Hilfsfunktion zum Einfügen, außerhalb der Klasse muss spezifiziert werden das add Teil des Klassentemplates ist (ADS_set<Key,N>
void ADS_set<Key, N>::add(const key_type& key) {//add als Teil des Templates ADS_set, mit Template Parametern Key und N, Die Funktion akzeptiert einen konstranen Refernzparameter key, der einzufügenden Schlüsselwert darstellt
    size_type i = h(key); //wo muss der Wert hin in Tabelle? - Berechnet Hashwert für key, Hashwert = Stelle wohin Wert muss, berechnete Haswert wird an Variable idx (Index übergeben)
    Element* element = new Element{key, Mode::USED}; //neues Element
    if (!table[i]) { 					 				 //Wenn Bucket leer
        table[i] = element;	//Element einfügen
    } else {  //Wenn eine Kollision passiert wird Element am Ende der linked List eingefügt
        Element* current = table[i]; //temporäre Variable current vom typ Element wird erstellt und auf Adrese des ersten Elements der verketteten Liste (table[idx]) gesetzt 
        while (current->next) {	//solange durch Liste iterieren bis Ende gefunden
            current = current->next; //-> Operator um Auflösen des Zeigers auf das Objekt und den anschließenden Zugriff auf dessen Mitglieder
        }
        current->next = element;
    }
    ++current_size;
}

//_________Hilfsfunktion_locate___________//
template <typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::locate(Element* current, const key_type& key) const {
    if (current) {									//Wenn das aktuelle Element kein Nullptr
        if (key_equal{}(current->key, key)) {		//Wenn der Key des current Elements gleich mit dem gegebenen Key ist
            return 1;								//return 1 -> Key wurde gefunden
        }
        return locate(current->next, key);			//Sonst ein Element weiter gehen in linked List
    }
    return 0;										//return 0 -> Key nicht gefunden
}



//_________Insert_Funktion___________//
template <typename Key, size_t N>	// PH1 typename ADS_set<Key, N>:: = Specifier da Funktion außerhalb der Klasse
template <typename InputIt> void ADS_set<Key, N>::insert(InputIt first, InputIt last) { //Wir haben 2 Iteratoren und wollen den Bereich zwischen den zwei Iteratoren einfügen
    for (auto it{first}; it != last; ++it) { // daher Iteratorschleife mit Iterator it //first und last sind Iteratornamen
        if (!count(*it)) {			// count liefert 0 oder 1 zurück je nachdem ob wert schon da ist //wenn count 0 fügen wir ein
            reserve(current_size);	//eine Zahl von Werten mitgeben und Tabelle soll so groß sein das Platz für die Werte ist
            add(*it);	
        }
    }
}

//_________Dump_Funktion___________//
template <typename Key, size_t N>
void ADS_set<Key, N>::dump(std::ostream& o) const {
    o << "Table Size: " << table_size << "; Current Size: " << current_size << "\n";
    for (size_type i {0}; i < table_size; ++i) {
        o << i << ":";
        for (Element* element = table[i]; element != nullptr; element = element->next) {
            o << " -> " << element->key;
        }
        if (table[i] == nullptr) {
            o << " -> " << static_cast<int>(Mode::FREE);
        }
        o << "\n";
    }
}

//_________Reserve_Funktion___________//
template <typename Key, size_t N>
void ADS_set<Key, N>::reserve(size_type current_size) {		//Berechnen wie groß muss Tabelle sein damit n Werte reinpassen
    if (table_size * max_lf >= current_size) return;		// Wenn Tabelle überfüllt
    size_type new_table_size {table_size};

    while (new_table_size * max_lf < current_size) 			//Solange Tabelle für Werte zu klein ist verdoppeln wir Größe der Tabelle table_size
	++(new_table_size *= 2);								//Table Size mit ++ noch um 1 erhöht damit auch bei Startgröße 0 verdoppelt
    rehash(new_table_size);								
}

//_________Rehash_Funktion___________//
template <typename Key, size_t N>
void ADS_set<Key, N>::rehash(size_type new_table_size) {
    if (new_table_size <= table_size) 				//Checken ob new table size kleiner als aktuelle table size 
        return;										//Wenn dann muss nicht vergrößert werden

    size_type old_table_size = table_size; 			//Aktuelle Table Size abspeichern
    table_size = get_next_prime(new_table_size); 	//Tabelle auf die nächste Primzahl vergrößern
    Element** new_table = new Element*[table_size]{nullptr};	//Neue Tabelle mit der neuen Größe erstellen und Elemente bis nullptr initialisieren

    for (size_type i = 0; i < old_table_size; ++i) { //Über Elemente in alter Tabelle iterieren
        Element* current = table[i];				//Das aktuelle Element holen
        while (current) {							//Durch die Linked List des aktuellen index iterieren
            Element* next = current->next;			//Das nächte Element in Tabelle speichern
            size_type index = h(current->key);		//Index des Elements im neuen Table berechnen

            if (new_table[index] == nullptr) {		//Wenn der Bucket leer ist in der neuen Tabelle
                new_table[index] = current;			//Aktuelles Element einfügen
                current->next = nullptr;			//Nächster Pointer nullptr setzen (Ende der liste)
            } else {								//Wenn der Bucket nicht leer ist
                Element* tail = new_table[index];	//Den Tail (Ende) der linked list holen
                while (tail->next) {				//Das Letzte Element der Linked list finden
                    tail = tail->next;
                }
                tail->next = current;			//Aktuelles Element am Ende der Linked List einfügen
                current->next = nullptr;		//Pointer auf das Nächste ist nullptr
            }

            current = next;	//Ein Elemnt weiter gehen in alter Tabelle
        }
    }

    delete[] table; 	//Alte Tabelle löschen
    table = new_table;	//neue Tabelle = aktuelle Tabelle

    //Den load Factor prüfen und wenn nötig vergrößern
    float load_factor = static_cast<float>(current_size) / table_size; 	//Berechnung Load Factor
    if (load_factor > max_lf) {											//Wenn aktueller Größer als max LF
        // Nächte Primzahl kalkulieren
        size_type next_prime = get_next_prime(table_size);
        rehash(next_prime); //Rehash mit neuer tabellen Größe
    }
}

//_________Get_Next_Prime_Funktion___________//
template <typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::get_next_prime(size_type n) {
    if (n < 2)							//Wenn n is kleiner als 2, 2 zurückgeben = kleinste Primzahl
        return 2;

    size_type next_prime = n + 1;		//Bei n+1 starten und nächste Prim suchen
    while (!is_prime(next_prime)) {		//Schleife bis Prim ist gefunden
        ++next_prime;					
    }

    return next_prime;					//Primzahl zurückgeben
}

//_________Is_Prime_Funktion___________//
template <typename Key, size_t N> 		
bool ADS_set<Key, N>::is_prime(size_type n) { 
    if (n < 2)										//Wenn kleiner als 2 kann nicht Prim sein
        return false;

    for (size_type i = 2; i * i <= n; ++i) { 		//Schleife durchläuft alle Zahlen von 2 bis zur Quadratwurzel von n. Wenn alle Zahlen bis Quadaratwurzeln überprüft werden werden auch alle möglichen Kombinationen geprüft von i und j f+r die i*j = n
        if (n % i == 0)								//Wenn dabei Teilbarkeit festgestellt wird -> Keine Prim
            return false;
    }

    return true;
}


//_________Clear_Funktion___________//
template <typename Key, size_t N>
void ADS_set<Key, N>::clear() {
    ADS_set tmp;		//leeres ADS set erstellen
    swap(tmp);			//Sets tauschen
}

//_________Swap_Funktion___________//
template <typename Key, size_t N>
void ADS_set<Key, N>::swap(ADS_set &other) {
    using std::swap;							//Neue Instanzvariablen nicht vergessen in Swap
    swap(table, other.table);					//vertauschen der Instanzvariablen
    swap(table_size, other.table_size);
    swap(current_size, other.current_size);
    swap(max_lf, other.max_lf);
}

//_________Einzelwert Insert___________//
template <typename Key, size_t N>
std::pair<typename ADS_set<Key, N>::iterator, bool> ADS_set<Key, N>::insert(const key_type& key) {
    if (find(key) != end()) {
        return std::pair<iterator, bool>(find(key), false);
    }
    reserve(current_size + 1);
    add(key);
    return std::pair<iterator, bool>(find(key), true);
}

//_________Find_Funtion___________//
template <typename Key, size_t N>
typename ADS_set<Key, N>::iterator ADS_set<Key, N>::find(const key_type& key) const {
    for (Element* current {table[h(key)]}; current; current = current->next) {
        if (key_equal{}(current->key, key)) {
            return iterator(table, current, h(key), table_size);
        }
    }
    return end();
}

//_________Erase_Funtion___________//
template <typename Key, size_t N>
typename ADS_set<Key, N>::size_type ADS_set<Key, N>::erase(const key_type& key) {
    size_type i = h(key);
    Element* current = table[i];
    Element* last = nullptr;

    while (current && !key_equal{}(current->key, key)) {
        last = current;
        current = current->next;
    }
    if (current) {
        if (!last) {
            table[i] = current->next;
        } else {
            last->next = current->next;
        }
        current->next = nullptr;
        delete current;
        --current_size;
        return 1;
    }
    return 0;
}

template <typename Key, size_t N>
ADS_set<Key,N> &ADS_set<Key,N>::operator=(const ADS_set &other) {
    ADS_set tmp{other};
    swap(tmp);
    return *this;
}

template <typename Key, size_t N>
ADS_set<Key,N> &ADS_set<Key,N>::operator=(std::initializer_list<key_type> ilist) {
    ADS_set tmp{ilist};
    swap(tmp);
    return *this;
}

template <typename Key, size_t N>
ADS_set<Key,N>::ADS_set(const ADS_set &other) {
    rehash(other.table_size);
    for (const auto &k: other) add(k);
}


//Iterator Member Funktionen

//_________Skip_Funktion___________//
template <typename Key, size_t N>
void ADS_set<Key, N>::Iterator::skip() {
    while (i < table_size && !table[i]) { ++i; }
    element = (i < table_size) ? table[i] : nullptr;
}

template <typename Key, size_t N>
typename ADS_set<Key, N>::Iterator& ADS_set<Key, N>::Iterator::operator++() {
    if (element->next) {
        element = element->next;
    } else {
        ++i;
        skip();
    }
    return *this;
}

//_________Globale_Swap_Funktion___________//
template <typename Key, size_t N>
void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs) {
    lhs.swap(rhs);
}

#endif // ADS_SET_H
