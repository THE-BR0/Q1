//// tools.cc
// Auteur : Theo Brochier
// Version : 8.0
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include "tools.h"
using namespace std;
using namespace tools;



Cercle::Cercle(S2d centre, double rayon) : centre(centre), rayon(rayon) {}
double Cercle::dist_du_centre(const S2d& point) const {
	return hypot(point.x - centre.x, point.y - centre.y);
}
bool Cercle::point_appartient_cercle(const S2d &point) const {
	return dist_du_centre(point) <rayon - epsil_zero;
}
double tools::dist_deux_pts(const S2d& p1,const S2d& p2) {
	return hypot(p1.x - p2.x, p1.y - p2.y);
}
bool Cercle::cercle_intersecte_cercle(const Cercle& autre_cercle) const {
	return dist_deux_pts(autre_cercle.centre, centre)
											<	autre_cercle.rayon + rayon+epsil_zero;

}
bool Cercle::cercle_appartient_cercle(const Cercle &autre_cercle) const {
	return dist_deux_pts(autre_cercle.centre , centre)
											<  rayon-autre_cercle.rayon -epsil_zero;
}
void Cercle::change_rayon(double rayon) {
	this->rayon = rayon;

}
void Cercle::change_centre(S2d centre) {
	this->centre = centre;
}

Polar tools:: cartesianToPolar(S2d cart) {
	Polar polar{};
	polar.r = sqrt(cart.x * cart.x + cart.y * cart.y);
	polar.theta = atan2(cart.y, cart.x);
	return polar;
}
S2d tools:: polarToCartesian(Polar polar) {
	S2d cart{};
	cart.x = polar.r * cos(polar.theta);
	cart.y = polar.r * sin(polar.theta);
	return cart;
}
S2d tools::nextDestination(const S2d& current, const Polar& vecteurVitesse) {
	S2d delta = polarToCartesian(vecteurVitesse);
	return {current.x + delta.x, current.y + delta.y};
}

void tools::renormalisation(double& alpha) {
	while (alpha > M_PI) alpha -= 2 * M_PI;
	while (alpha <= -M_PI) alpha += 2 * M_PI;
}

void tools:: rebond(S2d current, Polar& VecteurVitesse) {
	Polar a = cartesianToPolar(current);
	double beta (a.theta);
	double alpha (VecteurVitesse.theta);
	VecteurVitesse.theta=M_PI+ beta-(alpha-beta);
	renormalisation(VecteurVitesse.theta);
}
bool tools::collisionEntreCercles(const Cercle& c1, const Cercle& c2) {
    return c1.cercle_intersecte_cercle(c2);
}

