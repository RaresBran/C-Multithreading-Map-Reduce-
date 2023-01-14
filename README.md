Bran Rares 331CC
APD - Tema 1
----------------

Am rezolvat tema folosind:
 - o structura ce este pasata functiilor map si reduce
 - doua mutex-uri folosite de thread-uri pentru a evita race condition
 - o bariera pentru ca thread-urile de tip reduce sa-si inceapa munca dupa ce thread-urile de tip map au terminat
 - un algoritm de binary search pentru a afla daca numarul citit din fisier este o putere perfecta.

Thread-urile de tip MAP disponibile blocheaza mutex-ul, iau un fisier din coada file_names (aflata in structura) apoi elibereaza mutex-ul si proceseaza fisierul luat. Daca un numar este o putere perfecta acesta este pus intr-un vector de set-uri (result) la indicele dat de exponent (de exemplu in setul result[2] daca numarul este de tipul x^2). La final fiecare thread stocheaza rezultatul in structura (in results).

Dupa ridicarea barierei thread-urile de tip REDUCE incep prin a alege un exponent (de la 2 la R + 2, unde R = numarul de thread-uri reduce) iar apoi fac operatia de uniune intre set-urile ce contin puteri perfecte cu acel exponent. Rezultatul consta in printarea numarului de elemente (prin definitie unice) ale seturilor rezultate (unul pentru fiecare exponent).