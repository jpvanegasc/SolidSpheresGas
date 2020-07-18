/**
 * 2D Monte Carlo by Metropolis algorithm
 */
#include"constants.h"

class Metropolis{
    private:
        double r[N][D] = {0};

        double calculate_energy(int n_particle, double x, double y);
        double calculate_r_min(int from, int destination);
    public:
        void initialize(CRandom &ran);
        void metropolis_step(double beta, CRandom &ran);
        double get_mean_r(void);
        void print(void);
};

/**
 * Calculates the energy for a given particle on a given postion using the Lennard-Jones interaction
 * potential.
 * 
 * @param n_particle: particle id that is being calculated
 * @params x, y: x and y positions of particle that is being calculated
 * 
 * @return Energy for a given particle
 */
double Metropolis::calculate_energy(int n_particle, double x0, double y0){
    double E = 0;

    for(int n=0; n<N; n++){
        if(n == n_particle) continue;

        double x_temp = r[n][0], y_temp = r[n][1];
        x_temp = x_temp - x0 - L*std::fabs((x_temp-x0)/L);
        y_temp = y_temp - y0 - L*std::fabs((y_temp-y0)/L);
        double r2 = x_temp*x_temp + y_temp*y_temp;

        E += 4*(std::pow(r2, -6.0) - std::pow(r2, -3.0));
    }

    return E;
}

/**
 * Calculates the minimum distance between a given pair of particles
 * 
 * @params from, destination: ids of the particles whose distance is being calculated
 * 
 * @return minimum distance
 */
double Metropolis::calculate_r_min(int from, int destination){
    double x1 = r[from][0], y1 = r[from][1];
    double x2 = r[destination][0], y2 = r[destination][1];
    return std::sqrt(
        (x1 - x2 - L*std::fabs((x1-x2)/L))*(x1 - x2 - L*std::fabs((x1-x2)/L)) +
        (y1 - y2 - L*std::fabs((y1-y2)/L))*(y1 - y2 - L*std::fabs((y1-y2)/L))
    );
}

/* Initialize with random positions */
void Metropolis::initialize(CRandom &ran){
    for(int n=0; n<N; n++){
        r[n][0] = L*ran.r(); r[n][1] = L*ran.r();
    }
}

/**
 * Moves a single particle using the Metropolis criteria, with periodic boundaries.
 * 
 * @param beta: Thermodynamic beta
 */
void Metropolis::metropolis_step(double beta, CRandom &ran){
    int n = (int)N*ran.r(); double x = r[n][0], y = r[n][1];
    double drx = dr*(2*ran.r()-1), dry = dr*(2*ran.r()-1);

    // Periodic boundaries
    double x_new = x + drx, y_new = y + dry;

    if(x_new < 0) x_new += L;
    else if(x_new > L) x_new -= L;

    if(y_new < 0) y_new += L;
    if(y_new > L) y_new -= L;

    // Metropolis criteria
    double dE = calculate_energy(n, x_new, y_new) - calculate_energy(n, x, y);

    if(dE <= 0){
        r[n][0] = x_new; r[n][1] = y_new;
        return;
    }
    else if(ran.r() < std::exp(-beta*dE)){
        r[n][0] = x_new; r[n][1] = y_new;
        return;
    }
}

/**
 * Calculates the mean minimum distance between all particles of the system in a given state
 * 
 * @return minimum mean distance
 */
double Metropolis::get_mean_r(void){
    double mean_r = 0;

    for(int n1=0; n1<N; n1++)
        for(int n2=n1+2; n2<N; n2++)
            mean_r += calculate_r_min(n1, n2);

    mean_r /= N_connections;

    return mean_r;
}

void Metropolis::print(void){
    for(int n=0; n<N; n++)
        std::cout << " , " << r[n][0] << "+0.1*cos(t)," << r[n][1] << "+0.1*sin(t)";
}
