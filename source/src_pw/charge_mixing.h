//=================================
// Mohan add 2010-02-01
//=================================
#ifndef CHARGE_MIXING_H
#define CHARGE_MIXING_H

//==================================
// (1) Plain Mixing
// (2) KerKer Mixing
// (3) Pulay Mixing
// (4) Modified Broden Mixing
//===================================
#include "../module_base/global_function.h"
#include "../module_base/global_variable.h"
#include "../module_base/matrix.h"
class Charge_Mixing
{
	public:
	Charge_Mixing();
	~Charge_Mixing();

//======================================
// General interfaces, in charge_mixing.cpp
//======================================
    void set_mixing
    (
        const std::string &mixing_mode_in,
        const double &mixing_beta_in,
        const int &mixing_ndim_in,
		const double &mixing_gg0_in
    );//mohan add mixing_gg0_in 2014-09-27

	double get_drho(double** rho, double** rho_save,
		std::complex<double>** rhog, std::complex<double>** rhog_save, const double nelec);

    void mix_rho(const int &iter,
		double** rho,
		double** rho_save,
		std::complex<double>** rhog,
		std::complex<double>** rhog_save
	);// mix rho

    //for Pulay method
    //if first electronic step, then reset charge mixing
	void reset(const bool final_scf);

    // extracting parameters
	// normally these parameters will not be used
	// outside charge mixing, but Exx is using them
    // as well as some other places
    const std::string &get_mixing_mode() const {return mixing_mode;}
    double get_mixing_beta() const {return mixing_beta;}
    int get_mixing_ndim() const {return mixing_ndim;}
    double get_mixing_gg0() const {return mixing_gg0;}

	int get_totstep() const {return totstep;}
	int get_rstep() const {return rstep;}
	int get_dstep() const {return dstep;}
	int get_idstep() const {return idstep;}
	double* get_alpha() const {return alpha;}

	private:

//======================================
// General parameters
//======================================
    std::string mixing_mode;
    double mixing_beta;
    int mixing_ndim;
	double mixing_gg0; //mohan add 2014-09-27

    bool new_e_iteration;

//======================================
// simple plain mixing method, in charge_mixing.cpp
//======================================
    void plain_mixing( double *rho_in, double *rho_save_in ) const;

    double rhog_dot_product(const std::complex<double>*const*const rhog1, const std::complex<double>*const*const rhog2) const;

//======================================
// Pulay mixing method, in charge_pulay.cpp
//======================================

    void Pulay_mixing(double** rho, double**rho_save);

	bool initp; // p stands for pulay algorithms
	void allocate_Pulay();
    void deallocate_Pulay();

    // auxiliary variables / subroutines
    int irstep; //mohan add 2012-02-10
	int idstep;
	int totstep;
	int rstep; // the record step;
	int dstep; // Delta step " dstep = rstep-1 ".
	double* alpha; // - sum (Abar * dRR)

	double*** Rrho;// Rrho(i) = rho(i) - rho_save(i), (GlobalV::NSPIN, rstep, pw.nrxx)
	double*** dRrho;// dRrho(i) = Rrho(i+1) - Rrho(i), (GlobalV::NSPIN, dstep, pw.nrxx)
	double*** drho;// drho(i)= rho_save(i+1) - rho_save2(i), (GlobalV::NSPIN, dstep, pw.nrxx)
	double** rho_save2;//rho_save: rho_in, rho_save2: rho_in(last step)
	std::complex<double>*** dF; // dF(i) = rhog(i) - rhog_save(i), (GlobalV::NSPIN, rstep, rhopw->npw)
	std::complex<double>*** dn; // dn(i) = rhog(i+1) - rhog(i), (GlobalV::NSPIN, rstep, rhopw->npw)
	
	ModuleBase::matrix Abar; // <dR_j|dR_i>^{-1}
	double* dRR; // <dR_j|R_m>
	
	void generate_datas(const int &irstep, const int &idstep, const int &totstep, double** rho, double** rho_save);
	void generate_Abar(ModuleBase::matrix &A)const;
	void inverse_preA(const int &dim, ModuleBase::matrix &preA)const;
	void inverse_real_symmetry_matrix(ModuleBase::matrix &A)const; // indicate the spin.
	void generate_dRR(const int &m);
	void generate_alpha();
	void generate_new_rho(const int &is,const int &m, double**rho, double** rho_save);

	void generate_residual_vector(double *residual, const double* rho_out, const double* rho_in)const;
	double calculate_residual_norm(double *residual1, double *residual2)const;

//======================================
// Broyden mixing method, in charge_broyden.cpp
//======================================

	void Simplified_Broyden_mixing(const int &iter,
		double** rho,
		double** rho_save,
		std::complex<double>** rhog,
		std::complex<double>** rhog_save); //qianrui created 2021-5-15

	bool initb; // b stands for Broyden algorithms.
	void allocate_Broyden();
	void deallocate_Broyden();

	ModuleBase::matrix beta; // (dstep, dstep)
};

#endif
