#pragma once

#include <cstddef>
#include <memory>
#include <vector>

/*

This class represents all of the data we have about a student. I've started with
simply what classes the student is in, but this will obviously grow quickly. I
just don't know what else we need to track for sure yet.

The sections that a student is in are indexed by integers of size_t

PS - if we track something like year, it's probably best to use enums rather
than "magic number" integers

*/

namespace small_world::io {

class Student {
public:

  enum class Career : uint16_t {
    GRAD, UGRD
  };

  enum class Program : uint16_t {
    GRAD, MASTR, CERT, UGRD, DOCT, UGRD2
  };

  enum class Organization : uint16_t {
    ARHM, ATEC, BBSC, EMGT, ENCS, ENCSBIEN, ENCSMECH, ENCSSYSM, EPPS, GENS, GRAD, MGMT, NSMT, NSMTBIOL, NSMTCHEM, NSMTMATH, UGRD
  };

  enum class Group : uint16_t {
    ARHM, ATEC, BBSC, EMGT, ENCS, EPPS, GENS, GRAD, MGMT, NSMT, UGRD
  };

  enum class Plan : uint16_t {
    ACCTBS, ACCTMS, ACCTMSNF, ACCTND, ACMBMBMS, ACN_MS, ACTSBS, ACTSMS, AHSTMA, AMS_BA, ARHMND, ASOCMS, ATCMBA, ATCMDR, ATCMMA, ATCMMFA, ATECBA, ATECMFA, AUD_AUD, BA__BS, BCBMMS, BCHMBS, BCM_DR, BCM_MS, BIDMCERT, BIO_BA, BIO_BS, BIO_ND, BIOTMS, BIOTND, BLBABS, BLCJBA, BLHCBS, BMEDCERT, BMENBS, BMENDR, BMENMS, BMENND, BSANMS, BSANMSNF, BSMBMBMS, CE__BS, CE__DR, CE__MS, CGNSDR, CGS_BS, CHEMBA, CHEMBS, CHEMDR, CHEMMS, CJBLBA, CLDPBS, CMSDDR, COMDMS, CRIMBA, CRIMDR, CRIMMS, CS__BS, CS__DR, CS__MSCS, CS__ND, CYSYCERT, DSC_BS, DTSCCERT, ECFNBS, ECONBA, ECONBS, ECONDR, ECONMS, ECONND, EE__BSEE, EE__DR, EE__MSEE, EE__ND, EMACBA, EMBAMBA, EMGTNDNF, ENMGMS, ENSYCERT, FERMMSNF, FIMBMBMS, FIN_BS, FIN_MS, FIN_MSNF, FNECBS, GEINCERT, GEOSBS, GEOSDR, GEOSMS, GEOSND, GIS_BS, GISCERT, GLBSBS, GLHRBS, GLIPBS, GLMKBS, GLSCBS, GRADND, GRU_ND, GSISDR, GSISMS, GSISND, HCBLBS, HCLMMS, HCLMMSNF, HCLMND, HCLMNDNF, HCMBBS, HCMGBS, HCMGNDNF, HCSTCERT, HDECMS, HI__DR, HI__MA, HISTBA, HISTMA, HITHCERT, HLMBMBMS, HLTHBS, HRGLBS, HRM_BS, HS__BA, HUASDR, HUASMA, HUHIDR, HUMADR, HUMAMA, HUMAND, HUSLDR, HUSLMA, HUSLND, IEMBMBMS, ILMBMBMS, IMS_DR, IMS_MS, IMS_ND, INASCERT, INENMS, INTSBS, IPECBA, IPECBS, IPECMS, IPGLBS, IS__BA, IS__BS, IS__MA, ITMBMBMS, LATSMA, LGMCERT, LIT_BA, LITRBA, LITRDR, LITRMA, LITRND, LOD_MSNF, MAMBMBMS, MAS_MS, MAS_MSNF, MAS_ND, MATHBA, MATHBS, MATHDR, MATHMS, MATHND, MB__BS, MBA_MBA, MBA_MBANF, MBA_ND, MBHCBS, MECHBS, MECHDR, MECHMS, MED_MAT, MITMMS, MKGLBS, MKMBMBMS, MPA_MPA, MPA_ND, MPP_MPP, MRKTBS, MRKTMS, MSC_DR, MSENDR, MSENMS, NPMCERT, NSC_BS, NWVECERT, ORCNCERT, PAFFBS, PAFFDR, PHILBA, PHYSBA, PHYSBS, PHYSDR, PHYSMS, PHYSND, PPOLBS, PPPEDR, PRCOCERT, PSCIBA, PSCIDR, PSCIMA, PSCLMA, PSLSMA, PSY_BS, PSYSDR, PSYSMS, SCE_MAT, SCE_ND, SCGLBS, SCMBMBMS, SCMTBS, SCMTMS, SCMTMSNF, SCMTND, SDARMS, SE__BS, SE__DR, SE__MS, SE__MSNF, SE__ND, SHRMCERT, SOC_BA, SPAUBS, STATDR, STATMS, SYENCERT, SYSMMS, SYSMMSNF, TE__DR, TE__MSTE, TED_ND, UGS_ND, UND_BA, UND_BS, VPA_BA, VPA_DR, VPA_MA, VPA_ND
  };

  enum class Subplan : uint16_t {
    NONE = 0, ACCT, ACCTO, AICMP_MDL, AM_ECN_SYS, AM_LGL_SYS, AM_WLD_COM, AMERICA_ST, AMME, ANIMATION, APPLD_MATH, APPLD_STAT, ARHM_FND, ART_HIST, ARTADMNMGT, ATEC_GEN, BBSC_FND, BSANO, BUS_AN, BUS_COMM, BUS_ECON, BUS_ISSUES, BUSINESS, CIRCUITS, COACHING, COGN_NEURO, COHORT, COMM, COMP_SYS, COUNSELING, CPA, CR_MED_ST, CRTV_PRCT, DANCE, DATA_SCI, DESIG_PROD, DEVICES, DYNSYSTCON, EDUC_FND, ENCS_FND, ENERGY_MGT, ENVR_ST, EPPS_FND, FILM, FIN, FTMBA, GAME_DEV, GAME_ST, GAMES, GAMING, GEN_BUS, GEN_CON, GEN_GLB, GEN_MATH, GEN_STAT, GENDER_ST, GENERAL, GLEMBA, HCMPROF, HEALTHCARE, HIST_GEN, HLTH_WELL, HOL_LDRSHP, HOL_MGMT, HR, HS_GEN, INFOSYS, INNOV_ENTR, INSURANCE, INT_ARTS, INT_POL_EC, INTL_RELAT, IS_FND, LAW, LAW_LGL_CL, LAWCOURTS, LEGIS_ST, MANAGEMENT, MANUDESIN, MECH_MAT, MEDIA_COMM, MGMT_FND, MGTINFOSYS, MRKT, MUSIC, NGT_MDT, NSMT_FND, NTWKD_CL, ONLINE, OPMGT, ORG_BEHAVR, ORG_CNSLT, PARKLAND, PHILOSOPHY, PHT_VID_DG, PMBAE, PMBAH, PMBAO, PMBAX, PRJMGMT, PSYHCI, PUBLIC_REL, PWRELECEN, REAL_EST, RISK_INS, SALES, SCM, SIGN_SYST, SOC_ISSUES, STATISTICS, STRG_HR, TEACHCERT, TEACHISSUE, THEATRE, THERMFLUID, TRNSF_LDR, URBAN_ST, US_WORLD, UTSW, VISUAL_ART, WOMEN_ST
  };

  Student(std::shared_ptr<const std::vector<std::size_t>> sections,
          std::size_t unique_val,
          std::size_t stream,
          Career career,
          Program program,
          Organization organization,
          Group group,
          Plan plan,
          Subplan subplan);

  virtual ~Student();

  virtual std::shared_ptr<const std::vector<std::size_t>> get_sections() const final;

  virtual std::size_t get_unique_val() const final;
  virtual std::size_t get_stream() const final;
  virtual Career get_career() const final;
  virtual Program get_program() const final;
  virtual Organization get_organization() const final;
  virtual Group get_group() const final;
  virtual Plan get_plan() const final;
  virtual Subplan get_subplan() const final;

private:
  std::shared_ptr<const std::vector<std::size_t>> sections;

  const std::size_t unique_val;
  const std::size_t stream;
  const Career career;
  const Program program;
  const Organization organization;
  const Group group;
  const Plan plan;
  const Subplan subplan;
};

}

bool operator==(const small_world::io::Student& l, const small_world::io::Student& r);
bool operator!=(const small_world::io::Student& l, const small_world::io::Student& r);
