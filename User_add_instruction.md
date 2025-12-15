# Instrukcja Zarządzania Kartami RFID

System zamka RFID pozwala na dynamiczne dodawanie i usuwanie kart dostępu bez potrzeby modyfikacji kodu źródłowego. Do zarządzania uprawnieniami służy specjalna **Karta Master**.

## Konfiguracja Początkowa (Pierwsze Uruchomienie)

Przy pierwszym uruchomieniu systemu lub po wyczyszczeniu pamięci, **pierwsza zeskanowana karta automatycznie staje się Kartą Master**. Karta ta będzie od teraz służyć wyłącznie do autoryzacji zmian w systemie.

## 1. Dodawanie Nowej Karty Użytkownika

Aby dodać nową kartę (lub brelok), która będzie mogła otwierać zamek, wykonaj poniższe kroki:

1.  **Wejdź w tryb programowania:**
    *   Zbliż **Kartę Master** do czytnika.
    *   System potwierdzi wejście w tryb programowania długim sygnałem dźwiękowym.

2.  **Zarejestruj nową kartę:**
    *   Zbliż nową kartę, którą chcesz autoryzować.
    *   System potwierdzi dodanie karty krótkim sygnałem dźwiękowym.
    *   Możesz teraz dodać kolejne karty, powtarzając ten krok.

3.  **Wyjdź z trybu programowania:**
    *   Ponownie zbliż **Kartę Master** do czytnika.
    *   System wyda sygnał potwierdzający i wróci do normalnego trybu pracy.

> **Uwaga:** Jeśli spróbujesz dodać kartę, która już istnieje w bazie, system zasygnalizuje błąd (np. potrójnym sygnałem dźwiękowym).

## 2. Usuwanie Karty Użytkownika

1.  **Wejdź w tryb programowania** za pomocą **Karty Master**.

2.  **Zarejestruj kartę do usunięcia:**
    *   Zbliż kartę, której chcesz odebrać uprawnienia.
    *   System potwierdzi usunięcie karty z bazy danych (np. dwoma krótkimi sygnałami).

3.  **Wyjdź z trybu programowania**, ponownie przykładając **Kartę Master**.

## 3. Reset Awaryjny (Wyczyszczenie Bazy Danych)

W przypadku zgubienia Karty Master lub potrzeby całkowitego zresetowania systemu:

1.  Odłącz zasilanie od Arduino.
2.  Naciśnij i przytrzymaj fizyczny przycisk awaryjny.
3.  Trzymając przycisk, podłącz zasilanie do Arduino.
4.  Przytrzymaj przycisk przez około 10 sekund, aż system wyda sygnał potwierdzający wyczyszczenie pamięci EEPROM.

Po tej operacji system jest czysty, a **pierwsza przyłożona karta znów zostanie Kartą Master**.
