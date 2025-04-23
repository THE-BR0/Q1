// jeu.h
// Auteur : selim Ben Tkhayat
// Version : 5.0
#include "jeu.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <memory>
#include <vector>   // Ajout pour std::vector
#include <iomanip>  // Ajout pour std::setprecision, std::fixed
#include "tools.h"
#include "mobile.h"
#include "chaine.h"
#include "message.h"
using namespace std;
using namespace tools;
Jeu::Jeu() : score(0), chaine(S2d{}, r_capture),
arene({0.0, 0.0}, r_max), mode() {
}

// Mettre à jour la signature et l'initialisation
Jeu::Jeu(unsigned int score, vector<unique_ptr<mobile::Particule>>&& particules,
         vector<unique_ptr<mobile::Faiseur>>&& faiseurs, Chaine chaine, Cercle arene, Mode mode)
    : score(score), particules(move(particules)), faiseurs(move(faiseurs)),
      chaine(chaine), arene(arene), mode(mode) {} // Utiliser move pour transférer la propriété


void Jeu::set_score(unsigned int newScore) {
    score = newScore;
}

/**
 * Réinitialise complètement l'état du jeu
 * Remet le score à zéro et supprime tous les objets
 */
void Jeu::reset() {
    score = 0;
    particules.clear();
    faiseurs.clear();
    chaine.clear_articulations();
}


bool Jeu::lecture(const string& nomFichier) {
    ifstream file(nomFichier);
    reset(); // Réinitialisation préalable

    if (!file.is_open()) {
        cout << "Erreur: Impossible d'ouvrir le fichier " << nomFichier << endl;
        return false; // Échec de l'ouverture
    }

    string line;
    enum Section {
        SCORE, NB_PARTICULES, PARTICULES, NB_FAISEURS, FAISEURS, NB_ARTICULATIONS,
        ARTICULATIONS, MODE, FIN
    };
    Section section = SCORE;
    int nbPart = 0, nbFais = 0, nbArt = 0;
    size_t compteur_part_lues = 0;
    size_t compteur_fais_lus = 0;
    size_t compteur_art_lues = 0;

    auto passerAuxDonnees = [&](int nombre, Section actuelle, Section suivante,
                                Section saut) {
        section = (nombre == 0) ? saut : suivante;
    };

    while (section != FIN && getline(file, line)) {
        if (line.empty() || line[0] == '#' || line.find_first_not_of(" \t\r\n") ==
            string::npos) continue; // Ignore lignes vides/commentaires

        istringstream iss(line);
        iss >> ws;
        switch (section) {
            case SCORE:
                if (!decodage_score(iss)) {
                    cout << message::score_outside(score); // Message d'erreur déjà dans decodage
                    reset(); // Nettoyer en cas d'erreur
                    return false;
                }
                section = NB_PARTICULES;
                break;

            case NB_PARTICULES:
                if (!(iss >> nbPart) || nbPart < 0 ||
                    static_cast<size_t>(nbPart) > nb_particule_max) {
                    cout << message::nb_particule_outside(nbPart);
                    reset();
                    return false;
                }
                passerAuxDonnees(nbPart, NB_PARTICULES, PARTICULES, NB_FAISEURS);
                break;

            case PARTICULES:
                if (!decodage_particule(iss)) {
                    // Message d'erreur déjà dans decodage_particule
                    reset();
                    return false;
                }
                compteur_part_lues++;
                if (compteur_part_lues >= static_cast<size_t>(nbPart)) {
                    section = NB_FAISEURS;
                }
                break;

            case NB_FAISEURS:
                 if (!(iss >> nbFais) || nbFais < 0) {
                    cout << message::faiseur_nbe(nbFais) << endl;
                    reset();
                    return false;
                }
                passerAuxDonnees(nbFais, NB_FAISEURS, FAISEURS, NB_ARTICULATIONS);
                break;

            case FAISEURS:
                if (!decodage_faiseur(iss)) {
                     // Message d'erreur déjà dans decodage_faiseur ou verifier_collisions
                    reset();
                    return false;
                }
                compteur_fais_lus++;
                if (compteur_fais_lus >= static_cast<size_t>(nbFais)) {
                    // Vérifier les collisions inter-faiseurs UNE SEULE FOIS après lecture
                    if (!verifier_collisions_faiseurs()) {
                        // Message déjà affiché dans verifier_collisions_faiseurs
                        reset();
                        return false;
                    }
                    section = NB_ARTICULATIONS;
                }
                break;

            case NB_ARTICULATIONS:
                if (!(iss >> nbArt) || nbArt < 0) {
                    cout << "Erreur: Nombre d'articulations invalide (" << nbArt << ")."
                         << endl;
                    reset();
                    return false;
                }
                 // Vérifier si le nombre d'articulations est cohérent avec la chaîne
                 // (une chaîne vide doit avoir 0 articulations)
                 if (nbArt > 0 && chaine.getArticulations().empty() && nbArt != 1) {
                     // Si on attend des articulations mais que la chaîne est vide
                     // et qu'on n'est pas en train de lire la première.
                     // Ou si nbArt == 0 mais la chaîne n'est pas vide (géré par reset)
                 }
                passerAuxDonnees(nbArt, NB_ARTICULATIONS, ARTICULATIONS, MODE);
                break;

            case ARTICULATIONS:
                if (!decodage_chaine(iss)) {
                    // Message d'erreur déjà dans decodage_chaine
                    reset();
                    return false;
                }
                compteur_art_lues++;
                if (compteur_art_lues >= static_cast<size_t>(nbArt)) {
                     // Validation post-articulations (collision avec faiseurs)
                     // est déjà faite dans decodage_chaine pour chaque articulation.
                     // On pourrait ajouter une validation globale de la chaîne ici si nécessaire.
                    section = MODE;
                }
                break;

            case MODE: {
                string modeStr;
                if (!(iss >> modeStr) || (modeStr != "CONSTRUCTION" && modeStr != "GUIDAGE")) {
                     reset();
                     return false;
                }
                mode = (modeStr == "CONSTRUCTION") ? CONSTRUCTION : GUIDAGE;
                section = FIN; // Fin de la lecture attendue
                break;
            }

            case FIN:
                // Ne devrait pas arriver ici si le fichier est bien formé
                break;
        }
         // Vérifier s'il reste des caractères non lus sur la ligne (sauf pour FIN)
         if (section != FIN && !(iss >> std::ws).eof()) {
             cout << "Erreur: Caractères supplémentaires invalides sur la ligne: " << line << endl;
             reset();
             return false;
         }
    }

    // Vérifications finales après la boucle
    if (section != FIN && section != MODE) { // Si on n'a pas atteint la section MODE ou FIN
         cout << "Erreur: Fin de fichier inattendue ou section manquante." << endl;
         reset();
         return false;
    }
     if (particules.size() != static_cast<size_t>(nbPart)) {
         cout << "Erreur: Nombre de particules lu (" << particules.size()
              << ") ne correspond pas au nombre attendu (" << nbPart << ")." << endl;
         reset();
         return false;
     }
     if (faiseurs.size() != static_cast<size_t>(nbFais)) {
         cout << "Erreur: Nombre de faiseurs lu (" << faiseurs.size()
              << ") ne correspond pas au nombre attendu (" << nbFais << ")." << endl;
         reset();
         return false;
     }
     if (chaine.getArticulations().size() != static_cast<size_t>(nbArt)) {
         cout << "Erreur: Nombre d'articulations lues (" << chaine.getArticulations().size()
              << ") ne correspond pas au nombre attendu (" << nbArt << ")." << endl;
         reset();
         return false;
     }


    cout << message::success() << endl;
    file.close(); // Fermeture explicite (bien que RAII s'en charge)
    return true; // Succès
}

/**
 * Vérifie et charge le score depuis le flux d'entrée
 *  data Flux contenant le score
 * @return true si le score est valide (entre 1 et SCORE_MAX), false sinon.
 */
bool Jeu::decodage_score(istringstream& data) {
    // Tenter de lire le score
    if (!(data >> score)) {
        score = 0;//valeur par defaut
        cout << message::score_outside(score) << endl;
        return false; // Échec de la lecture
    }
    // Vérifier la validité du score
    if (score > score_max || score <= 0) {
        cout << message::score_outside(score) << endl;
        return false; // Score invalide
    }
    return true;
}

/**
 * Lit et crée une particule à partir du flux d'entrée
 * Vérifie la validité des paramètres (position dans l'arène, vitesse, compteur)
 *  data Flux contenant les données de la particule
 * @return true si la lecture et la validation réussissent, false sinon.
 */
bool Jeu::decodage_particule(istringstream& data) {
    double x, y, angle, distance;
    unsigned compteur;
    // Tenter de lire toutes les données
    if (!(data >> x >> y >> angle >> distance >> compteur)) {
        cout << "Erreur: Format de ligne de particule incorrect." << endl;
        return false;
    }

    renormalisation(angle);
    S2d pos{x, y};
    Polar vit{distance,angle};

    if (!arene.point_appartient_cercle(pos)) {
        cout << message::particule_outside(x, y) << endl;
        return false;
    }

    if (distance < 0 || distance > d_max) {
        cout << message::mobile_displacement(distance) << endl;
        return false;
    }

    if (compteur >= time_to_split) { // compteur est unsigned, donc >= 0
        cout << message::particule_counter(compteur) << endl;
        return false;
    }

    // Créer et ajouter un unique_ptr
    particules.emplace_back(make_unique<mobile::Particule>(pos, vit, compteur));
    return true; // Succès
}

bool Jeu::decodage_faiseur(istringstream& data) {
    double x, y, a, d, r;  // Coordonnées, angle, distance, rayon
    int taille;            // Taille du faiseur

    // Tenter de lire toutes les données
    if (!(data >> x >> y >> a >> d >> r >> taille)) {
        cout << "Erreur: Format de ligne de faiseur incorrect." << endl;
        return false;
    }

    // Vérification que la taille est valide
    if (taille <= 0) {
        cout << message::faiseur_nbe(taille) << endl;
        return false;
    }

    if (d < 0 || d > d_max) {
        cout << message::mobile_displacement(d) << endl;
        return false;
    }

    if (r < r_min_faiseur || r > r_max_faiseur) {
        cout << message::faiseur_radius(r) << endl;
        return false;
    }

    S2d pos{x, y};
    Polar vit{d, a}; // Attention: l'ordre dans Polar est {distance, angle}

    // Créer temporairement pour vérifier l'appartenance à l'arène
    auto temp_faiseur_ptr = make_unique<mobile::Faiseur>(pos, vit, r, taille);
    temp_faiseur_ptr->initialisation_corps(); // Initialiser pour vérifier le corps

    // Vérifier si le centre et tous les éléments du corps sont dans l'arène
    if (!arene.cercle_appartient_cercle({pos, r})) { // Vérifie le cercle englobant
        cout << message::faiseur_outside(x, y) << endl;
        return false;
    }
    const auto& corps = temp_faiseur_ptr->get_corps();
    for (const auto& corps_individuel : corps) {
        if (!arene.cercle_appartient_cercle(corps_individuel)) {
            const auto& centre_corp = corps_individuel.get_centre();
            cout << message::faiseur_outside(centre_corp.x, centre_corp.y) << endl;
            return false;
        }
    }

    // Si tout est valide, ajouter le faiseur au vecteur
    faiseurs.push_back(std::move(temp_faiseur_ptr));

    // La vérification des collisions inter-faiseurs est faite APRES lecture de tous les faiseurs
    // La vérification des collisions avec les articulations est faite dans decodage_chaine

    return true; // Succès
}

bool Jeu::decodage_chaine(istringstream& data) {
    double x, y;
    // Tenter de lire les coordonnées
    if (!(data >> x >> y)) {
        cout << "Erreur: Format de ligne d'articulation incorrect." << endl;
        return false;
    }

    const S2d pos{x, y};
    auto& arts = chaine.getArticulations(); // Référence au vecteur existant

    // 1. Validation: Position dans l'arène
    if (!arene.point_appartient_cercle(pos)) {
        cout << message::articulation_outside(x, y) << endl;
        return false;
    }

    // 2. Validation: Contraintes de la chaîne (racine ou inter-articulations)
    if (arts.empty()) { // Si c'est la première articulation (racine)
        double dist_au_centre = dist_deux_pts(pos, arene.get_centre());
        // La racine doit être sur le bord extérieur de la zone de capture
        if (abs(dist_au_centre - (r_max - r_capture / 2.0)) > r_capture / 2.0 + epsil_zero) {
             // Permet une tolérance autour du cercle r_max - r_capture
             // Ajustement potentiel nécessaire selon la définition exacte attendue
            cout << message::chaine_racine(pos.x, pos.y) << endl;
            return false;
        }
    } else { // Si ce n'est pas la première articulation
        // Vérifier la distance avec l'articulation précédente
        double dist_prec = dist_deux_pts(arts.back(), pos);
        if (dist_prec > r_capture + epsil_zero) { // Ajouter tolérance
            cout << message::chaine_max_distance(static_cast<unsigned>(arts.size())) << endl;
            // L'indice du message est celui de l'articulation qu'on essaie d'ajouter
            return false;
        }
         if (dist_prec < epsil_zero) { // Vérifier si superposée à la précédente
             cout << "Erreur: Articulation " << arts.size()
                  << " superposée à la précédente." << endl;
             return false;
         }
    }

    // 3. Validation: Collision avec les faiseurs EXISTANTS
    for (size_t i = 0; i < faiseurs.size(); ++i) {
        const auto& corps_faiseur = faiseurs[i]->get_corps();
        for (size_t j = 0; j < corps_faiseur.size(); ++j) {
            if (corps_faiseur[j].point_appartient_cercle(pos)) {
                cout << message::chaine_articulation_collision(arts.size(), i, j) << endl;
                // L'indice de l'articulation est celui qu'elle AURAIT si ajoutée
                return false;
            }
        }
    }

    // Si toutes les validations passent, ajouter l'articulation
    chaine.ajouterArticulation(pos);
    return true; // Succès
}

/**
 * Vérifie les collisions entre tous les éléments de tous les faiseurs.
 * @return true s'il n'y a AUCUNE collision, false s'il y a au moins une collision.
 */
bool Jeu::verifier_collisions_faiseurs() const {
    for (size_t i = 0; i < faiseurs.size(); ++i) {
        for (size_t j = i + 1; j < faiseurs.size(); ++j) {
            const auto& corps1 = faiseurs[i]->get_corps();
            const auto& corps2 = faiseurs[j]->get_corps();

            for (size_t k = 0; k < corps1.size(); ++k) {
                for (size_t l = 0; l < corps2.size(); ++l) {
                    // Utiliser la collision de cercles fournie par geomod si disponible
                    // ou calculer manuellement comme avant.
                    if (tools::collisionEntreCercles(corps1[k], corps2[l])) {
                         cout << message::faiseur_element_collision(i, k, j, l) << endl;
                         return false; // Collision détectée
                    }
                }
            }
        }
    }
    return true; // Aucune collision détectée
}

bool Jeu::sauvegarder(const std::string& nom_fichier) const{
    ofstream file(nom_fichier);
    if (!file.is_open()) {
        cout << "Erreur: Impossible d'ouvrir le fichier " << nom_fichier << endl;
        return false; // Échec de l'ouverture
    }

    // Écrire le score
    file << score << endl;

    // Écrire les particules
    file << particules.size() << endl;
    for (const auto& particule : particules) {
        const auto& pos = particule->get_position();
        const auto& vit = particule->get_vitesse();
        file << pos.x << " " << pos.y << " "
             << vit.theta << " " << vit.r << " "
             << particule->get_compteur() << endl;
    }

    // Écrire les faiseurs
    file << faiseurs.size() << endl;
    for (const auto& faiseur : faiseurs) {
        const auto& pos = faiseur->get_position();
        const auto& vit = faiseur->get_vitesse();
        file << pos.x << " " << pos.y << " "
             << vit.theta << " " << vit.r << " "
             << faiseur->get_rayon() << " "
             << faiseur->get_taille() << endl;
    }

    // Écrire les articulations de la chaîne
    file << chaine.getArticulations().size() << endl;
    for (const auto& articulation : chaine.getArticulations()) {
        file << articulation.x << " " << articulation.y << endl;
    }

    // Écrire le mode
    file << (mode == CONSTRUCTION ? "CONSTRUCTION" : "GUIDAGE") << endl;

    file.close(); // Fermeture explicite
    return true; // Succès
}