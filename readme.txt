Ciobanu Dorin 321 CB. Tema 2.

Pentru rezolvarea acestei teme am scris in doua fisiere subscriber.c si server.c

- Pentru implementarea clientului/subscriber-ului am implementat un mult ceea ce
s-a facut la laboratoarele precedente. Pentru a rezolva problema cu mai multi
clienti se foloseste structura FD SET, in care se decine toti clientii. Am deschis
un socket "sockfd" pentru clientul meu, am setat FD SET cu zero initial,
dupa am completat structura de tip sockaddr_in pentru informatiile utile de PORT,
IP Server si familia. Dupa ce am completat campurile structurii am  incercat sa ma
conectez la socket prin "connect". Pentru a afisa outputl dorit, am decis sa trimit
un mesaj serverului ca acesta sa-mi afiseze bine ID meu. Intr-o bucla infinita am 
selectat socket maxim, facut o copiet a FD Setului. Apoi verific daca vreau sa 
citesc ceva de la tastatura si sa trimit serverului sau trebuie sa primesc eu ceva
de la server. Daca trebuie sa primesc ceva de la server, atunci ar trebui sa afisez
mesajele cu notificarile necesare. Aici am folosit "recv" si am stocat informatia
in buffer, api in afisez.
- Intru cat clientul poate sa trimita doar 3 mesaje, am tratat pe toate aparte in
if-uri diferite. Daca s-a introdus ceva invafara de exit/sub/(un)sub afisez 
clientului ca a gresit mesajul si sa introduca ceva din aceste trei comenzi. Apoit
 trimit serverului mesajul. Daca e exit sa se deconecteze serverul (insa nu inteleg
  dc la un moment dat a incetat sa mearga comanda de exit avand un seg fault).


- Pentru imlementarea serverului am decis sa deschid 2 socketi, pentru TCP si UDP
(clienti). La fel, creez socketii pentru a lucra cu ei, actualizez campurile structurii
sockaddr_in. Fac bind la socketi pentru structura sockaddr_in. Intr-o bucla de while
am setat pe zero FD Setul, am incarcat socketii de UDP si de server. Selelctez
cel mai mare socket. Dupa ce s-a acceptat o conexiune la socket, afisez informatia
primita de la client cine s-a connectat si cu ce ID. Daca s-a connectat cine la 
server, adaug socketul acestuia utilizator in vecotrul de scoketi.
	Daca serverul meu a primit un mesaj UDP atunci eu trebuie sa-i trmit clientilor
	abonati. Mesajul UDP a venit intr-un string pe care l-am parsat si, parsand 
	toata informatia am creat un string (buffer) pe care pot sa-l trimit clientului
	si acesta sa afiseze la stdout deja in formatul potrivit. (Din pacate nu am 
	facut partea in care un client poate sa se aboneze la mai multe topicuri, ci 
	doar la unul.). 
	Daca am primit un mesaj de la clientul TCP sa acesta vrea sa se aboneze sau sa 
	se dezaboneze... Daca clientul doreste sa se aboneze, ii adaug in campul de 
	string "topic", denumirea topicilui corespunzator. Daca doreste sa se dezaboneze
	il sterg din lista. Si la fel, trimit niste notificari clientilor, ca acestea 
	sau abonat/dezabonat. 


	P.S. Dupa parerea mea am implement o mica parte din tema, mai pe scrut, am implementat dartea de client si un pic din partea de server (primirea mesajelor UDP + parsarea). Si abonarea clientilor doar la un singur topic de la care acestea pot sa primeasca notificari, fara S&F.