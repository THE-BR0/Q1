//// tools.h
// Auteur : Theo Brochier
// Version : 8.0
#ifndef TOOLS_H
#define TOOLS_H
constexpr double epsil_zero = 0.0;

namespace tools {
    struct S2d {
        double x;
        double y;
    };
    struct Polar {
        double r;
        double theta;
    };
    class Cercle {
    public:
        Cercle()
            : centre({0.0, 0.0}), rayon(0.0) {}
        Cercle(S2d centre, double rayon);

        double dist_du_centre(const S2d& point) const;
        bool point_appartient_cercle(const S2d& point) const;
        bool cercle_appartient_cercle(const Cercle& autre_cercle) const;
        bool cercle_intersecte_cercle(const Cercle& autre_cercle) const;
        void change_rayon(double rayon);
        void change_centre(S2d centre);
        S2d get_centre() const { return centre; }
        double get_rayon() const { return rayon; }

    private:
        S2d centre;   // Centre du cercle
        double rayon; // Rayon du cercle
    };
    double dist_deux_pts(const S2d& p1,const S2d& p2);
    Polar cartesianToPolar(S2d cart);
    S2d polarToCartesian( Polar polar);
    S2d nextDestination (const S2d& current, const Polar& vecteurVitesse);
    void renormalisation (double& alpha);
    void rebond(S2d current, Polar& VecteurVitesse) ;
    bool collisionEntreCercles(const Cercle& c1, const Cercle& c2);

}


#endif
