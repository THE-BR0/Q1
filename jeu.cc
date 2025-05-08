// jeu.cc
// Auteur : selim Ben Tkhayat
// Version : 7.0
#include "jeu.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <memory>
#include <vector>   
#include <iomanip>  
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
      chaine(chaine), arene(arene), mode(mode),statut(ONGOING) {} 


void Jeu::set_score(unsigned int newScore) {
    score = newScore;
}


//Réinitialise complètement l'état du jeu
void Jeu::reset() {
    score = 0;
    particules.clear();
    faiseurs.clear();
    chaine.clear_articulations();
}


bool Jeu::lecture(const string& nomFichier) {
    ifstream file(nomFichier);
    reset(); 
    if (!file.is_open()) {
        return false;}
    string line;
    enum Section {
        SCORE, NB_PARTICULES, PARTICULES, NB_FAISEURS, FAISEURS, NB_ARTICULATIONS,
        ARTICULATIONS, MODE, FIN};
    Section section = SCORE;
    int nbPart = 0, nbFais = 0, nbArt = 0;
    size_t compteur_part_lues = 0,compteur_fais_lus = 0,compteur_art_lues = 0;
    auto passerAuxDonnees = [&](int nombre, Section actuelle, Section suivante,
                                Section saut) {
        section = (nombre == 0) ? saut : suivante;};
    while (section != FIN && getline(file, line)) {
        if (line.empty() || line[0] == '#' || line.find_first_not_of(" \t\r\n") ==
            string::npos) continue; // Ignore lignes vides/commentaires
        istringstream iss(line);
        iss >> ws;
        switch (section) {
            case SCORE:
                if (!decodage_score(iss)) {
                    cout << message::score_outside(score); 
                    reset();return false;}
                section = NB_PARTICULES;
                break;
            case NB_PARTICULES:
                if (!(iss >> nbPart) || nbPart < 0 ||
                    static_cast<size_t>(nbPart) > nb_particule_max) {
                    cout << message::nb_particule_outside(nbPart);
                    reset();return false;}
                passerAuxDonnees(nbPart, NB_PARTICULES, PARTICULES, NB_FAISEURS);
                break;
            case PARTICULES:
                if (!decodage_particule(iss)) {
                    reset();return false;}
                compteur_part_lues++;
                if (compteur_part_lues >= static_cast<size_t>(nbPart)) {
                    section = NB_FAISEURS;}
                break;
            case NB_FAISEURS:
                 if (!(iss >> nbFais) || nbFais < 0) {
                    cout << message::faiseur_nbe(nbFais) << endl;
                    reset();return false;}
                passerAuxDonnees(nbFais, NB_FAISEURS, FAISEURS, NB_ARTICULATIONS);
                break;
            case FAISEURS:
                if (!decodage_faiseur(iss)) {
                    reset();
                    return false;}
                compteur_fais_lus++;
                if (compteur_fais_lus >= static_cast<size_t>(nbFais)) {
                    if (!verifier_collisions_faiseurs()) {
                        reset();return false;}
                    section = NB_ARTICULATIONS;}
                break;
            case NB_ARTICULATIONS:
                if (!(iss >> nbArt) || nbArt < 0) {
                    reset();return false; }
                passerAuxDonnees(nbArt, NB_ARTICULATIONS, ARTICULATIONS, MODE);
                break;
            case ARTICULATIONS:
                if (!decodage_chaine(iss)) {
                    reset();return false;}
                compteur_art_lues++;
                if (compteur_art_lues >= static_cast<size_t>(nbArt)) {
                    section = MODE;}
                break;
            case MODE: {
                string modeStr;
                if (!(iss>>modeStr)||(modeStr!="CONSTRUCTION"&&modeStr!="GUIDAGE")) {
                     reset();return false;}
                mode = (modeStr == "CONSTRUCTION") ? CONSTRUCTION : GUIDAGE;
                section = FIN; // Fin de la lecture attendue
                break;}
            case FIN:break;}}
    cout << message::success() << endl;
    file.close(); 
    return true; }


bool Jeu::decodage_score(istringstream& data) {
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
    Polar vit{d, a}; 

    // Créer temporairement pour vérifier l'appartenance à l'arène
    auto temp_faiseur_ptr = make_unique<mobile::Faiseur>(pos, vit, r, taille);
    temp_faiseur_ptr->initialisation_corps(); // Initialiser pour vérifier le corps

    // Vérifier si le centre et tous les éléments du corps sont dans l'arène
    if (!arene.cercle_appartient_cercle({pos, r})) { 
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

    return true; // Succès
}

bool Jeu::decodage_chaine(istringstream& data) {
    double x, y;
    // Tenter de lire les coordonnées x et y
    if (!(data >> x >> y)) {
        cout << "Format de ligne incorrect." << endl;
        return false; // Échec de la lecture
    }

    const S2d pos{x, y}; // Créer le point de l'articulation
    auto& arts = chaine.getArticulations(); // Référence au vecteur d'articulations

    // Vérifier si le point est dans l'arène
    if (!arene.point_appartient_cercle(pos)) {
        cout << message::articulation_outside(x, y) << endl;
        return false;
    }

    // Cas de la première articulation (racine de la chaîne)
    if (arts.empty()) {
        double dist_au_centre = dist_deux_pts(pos, arene.get_centre());
        // Vérifier si la racine est à la bonne distance du bord de l'arène
        if (dist_au_centre < r_max - r_capture || dist_au_centre > r_max) {
            cout << message::chaine_racine(pos.x, pos.y) << endl;
            return false;
        }
    } else { // Cas des articulations suivantes
        if (dist_deux_pts(arts.back(), pos) > r_capture) {
            cout << message::chaine_max_distance(static_cast<unsigned>(arts.size()));
            return false;
        }
    }

    // Vérifier les collisions avec les corps des faiseurs
    for (size_t i = 0; i < faiseurs.size(); ++i) {
        const auto& corps_faiseur = faiseurs[i]->get_corps();
        for (size_t j = 0; j < corps_faiseur.size(); ++j) {
            if (corps_faiseur[j].point_appartient_cercle(pos)) {
                cout << message::chaine_articulation_collision(arts.size(),i,j)<<endl;
                return false; 
            }
        }
    }

    // Si toutes les vérifications passent, ajouter l'articulation à la chaîne
    chaine.ajouterArticulation(pos);
    return true; 
}

/**
 * Vérifie les collisions entre tous les éléments de tous les faiseurs.
 * return true s'il n'y a AUCUNE collision, false s'il y a au moins une collision.
 */
bool Jeu::verifier_collisions_faiseurs() const {
    for (size_t i = 0; i < faiseurs.size(); ++i) {
        for (size_t j = i + 1; j < faiseurs.size(); ++j) {
            const auto& corps1 = faiseurs[i]->get_corps();
            const auto& corps2 = faiseurs[j]->get_corps();

            for (size_t k = 0; k < corps1.size(); ++k) {
                for (size_t l = 0; l < corps2.size(); ++l) {
                    if (tools::collisionEntreCercles(corps1[k], corps2[l])) {
                         cout << message::faiseur_element_collision(i, k, j, l)<<endl;
                         return false; 
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
string Jeu::get_status() const {
    switch(statut) {
        case ONGOING: 
            return "ONGOING";
        case WON: 
            return "WON";
        case LOST: 
            return "LOST";
            }
    return "UNKNOWN"; 
}
void Jeu::set_status(Status new_status) {
    statut = new_status;
}

void Jeu::ajouterFaiseur(const mobile::Faiseur& faiseur) {
    auto nouveau_faiseur = std::make_unique<mobile::Faiseur>(faiseur);
    
    tools::Cercle cercle_faiseur(nouveau_faiseur->get_position(), nouveau_faiseur->get_rayon());
    
    faiseurs.push_back(std::move(nouveau_faiseur));
}
void Jeu::dessiner() const {
    dessin_cercle(arene,GREEN);
    for (const auto& particule : particules){
        particule->dessin();
    }
    for (const auto& faiseur : faiseurs){
        faiseur->dessin();
    }
    chaine.dessin_chaine(RED);
}
void Jeu::update() {
    if (score > 0) {
        score--;
    } else {
        statut = LOST;
        return;
    }
    for (size_t i = 0; i < particules.size(); i++) {
        particules[i]->increment_compteur();

        if (particules[i]->get_compteur() >= time_to_split) {
            if (particules.size() >= nb_particule_max) {
                particules.erase(particules.begin() + i);
                i--;
            } else {
                decomposer_particule(i);
                i--;
            }
        } else {
            particules[i]->particule_deplacement(arene);
        }
    }
    vector<bool> faiseur_doit_sarreter(faiseurs.size(), false);
    for (size_t i = 0; i < faiseurs.size(); ++i) {
        for (size_t j = i + 1; j < faiseurs.size(); ++j) {
            // Collision tête-tête
            tools::Cercle tete_i(faiseurs[i]->get_position(),faiseurs[i]->get_rayon());
            tools::Cercle tete_j(faiseurs[j]->get_position(),faiseurs[j]->get_rayon());
            col_fais(tete_i, tete_j, faiseur_doit_sarreter, i, j);
        }
    }
    for (size_t i = 0; i < faiseurs.size(); ++i) {
        if (!faiseur_doit_sarreter[i]) {
            faiseurs[i]->faiseurs_deplacement(arene);
        }
    }
    //bool chaine_a_ete_detruite = false;
} 

void Jeu::col_fais(const tools::Cercle& tete_i, const tools::Cercle& tete_j, 
                  std::vector<bool>& faiseur_doit_sarreter, 
                  size_t i, size_t j) {
    if (tools::collisionEntreCercles(tete_i, tete_j)) {
        // Les deux têtes se touchent, les deux faiseurs s'arrêtent
        faiseur_doit_sarreter[i] = true;
        faiseur_doit_sarreter[j] = true;
        return; 
    }
    
    // Collision tête i avec corps j
    const auto& corps_j = faiseurs[j]->get_corps();
    for (const auto& segment : corps_j) {
        if (tools::collisionEntreCercles(tete_i, segment)) {
            faiseur_doit_sarreter[i] = true;
            break;
        }
    }
    
    // Collision tête j avec corps i
    const auto& corps_i = faiseurs[i]->get_corps();
    for (const auto& segment : corps_i) {
        if (tools::collisionEntreCercles(tete_j, segment)) {
            faiseur_doit_sarreter[j] = true;
            break;
        }
    }
}
void Jeu::decomposer_particule(size_t index) {
    S2d position = particules[index]->get_position();
    Polar vitesse = particules[index]->get_vitesse();
    
    Polar vitesse1 = vitesse;
    Polar vitesse2 = vitesse;
    
    vitesse1.theta += delta_split;
    vitesse2.theta -= delta_split;
    
    renormalisation(vitesse1.theta);
    renormalisation(vitesse2.theta);
    
    vitesse1.r *= coef_split;
    vitesse2.r *= coef_split;
    
    particules.erase(particules.begin() + index);
    
    particules.push_back(make_unique<mobile::Particule>(position, vitesse1, 1));
    particules.push_back(make_unique<mobile::Particule>(position, vitesse2, 1));


}


/*  Pour rendu3
void Jeu::executer_algorithme_guidage() {
   
}
*/