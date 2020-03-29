#include"constants.h"
#include"vector.h"

class Body{
    private:
        Vector3D r, V, F; double m, R;
    public:
        void initialize(double x0, double y0, double Vx0, double Vy0, double m, double R);
        void add_force(Vector3D dF);
        void move_r(double dt, double coef){r += V*(dt*coef);}
        void move_v(double dt, double coef){V += F*(dt*coef/m);}
        void print(void);
        void delete_f(void){F.load(0,0,0);}
        double get_x(void){return r.x();} 
        double get_y(void){return r.y();} 
        double get_z(void){return r.z();} 
        double get_vx(void){return V.x();} 
        double get_vy(void){return V.y();} 
        double get_fx(void){return F.x();} 
        double get_fy(void){return F.y();} 

        friend class Collider;
};

void Body::initialize(double x0, double y0, double Vx0, double Vy0, double m0, double R0){
    r.load(x0,y0,0); V.load(Vx0,Vy0,0); m = m0; R = R0;
}
void Body::add_force(Vector3D dF){
    double Fx=0, Fy=0, x=r.x(), y=r.y();
    
    double hx1 = R - std::fabs(0 - x);
    double hy1 = R - std::fabs(0 - y);
    
    if(hx1 > 0) Fx = 1e4*std::pow(hx1, 1.5);
    else if(hx1 < 0) {
        double hx2 = R - std::fabs(Lx - x);
        if(hx2 > 0) Fx = 1e4*std::pow(hx2, 1.5);
    }

    if(hy1 > 0) Fy = 1e4*std::pow(hy1, 1.5);
    else if(hy1 < 0) {
        double hy2 = R - std::fabs(Ly - y);;
        if(hy2 > 0) Fy = 1e4*std::pow(hy2, 1.5);
    }

    Vector3D aux(Fx, Fy, 0);
    F += dF + aux;
}
void Body::print(void){
    std::cout<<" , "<<r.x()<<"+"<<R<<"*cos(t),"<<r.y()<<"+"<<R<<"*sin(t)";
}

class Collider{
    public:
        void calculate_force_pair(Body &molecule1, Body &molecule2);
        void calculate_all_forces(Body *molecule);
        void move_with_pefrl(Body *molecule, double dt);
};

void Collider::calculate_force_pair(Body &molecule1, Body &molecule2){
    Vector3D dr = molecule2.r - molecule1.r;
    
    double s = (molecule1.R + molecule2.R) - norm(molecule1.r-molecule2.r);
    double F_aux = 1e4*std::pow(s, 1.5);

    Vector3D dF = dr*F_aux;
    molecule2.add_force(dF); molecule1.add_force((-1.0)*dF);
}

void Collider::calculate_all_forces(Body *molecule){
    for(int i=0; i<N; i++) molecule[i].delete_f();
    
    for(int i=0; i<N; i++)
        for(int j=i+1; j<N; j++)
            calculate_force_pair(molecule[i], molecule[j]);
}

void Collider::move_with_pefrl(Body *molecule, double dt){
    int i;
    for(i=0;i<N;i++) molecule[i].move_r(dt,Zi);
    calculate_all_forces(molecule);
    for(i=0;i<N;i++) molecule[i].move_v(dt,coef1);
    for(i=0;i<N;i++) molecule[i].move_r(dt,Xi);
    calculate_all_forces(molecule);
    for(i=0;i<N;i++) molecule[i].move_v(dt,Lambda);
    for(i=0;i<N;i++) molecule[i].move_r(dt,coef2);
    calculate_all_forces(molecule);
    for(i=0;i<N;i++) molecule[i].move_v(dt,Lambda);
    for(i=0;i<N;i++) molecule[i].move_r(dt,Xi);
    calculate_all_forces(molecule);
    for(i=0;i<N;i++) molecule[i].move_v(dt,coef1);
    for(i=0;i<N;i++) molecule[i].move_r(dt,Zi);
}