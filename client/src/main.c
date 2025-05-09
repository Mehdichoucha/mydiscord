#include <stdio.h>
#include <libpq-fe.h>

int main() {
    // Connexion à la base
    const char *conninfo = "host=localhost port=5432 dbname=ma_base user=mon_user password=mon_mdp";
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Erreur de connexion : %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    // Exécution d'une requête SQL
    PGresult *res = PQexec(conn, "SELECT id, nom FROM utilisateurs");

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Erreur dans la requête : %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 1;
    }

    // Affichage des résultats
    int nb_rows = PQntuples(res);
    for (int i = 0; i < nb_rows; i++) {
        printf("id: %s, nom: %s\n",
               PQgetvalue(res, i, 0),
               PQgetvalue(res, i, 1));
    }

    // Libération et fermeture
    PQclear(res);
    PQfinish(conn);
    return 0;
}
