#pragma once

#include <cstddef>
#include <ctime>
#include <memory>
#include <vector>

// Almost exactly like the Student class, but for Sections

namespace small_world::io {

class Section {
public:

  enum class Session : uint16_t {
    _1, _8W1, _8W2, AMN, BMN, CMN, DMN, FMN, Z01, Z02, Z03, Z04, Z05, Z06, Z07, Z08, Z09, Z10, Z11, Z12, Z13, Z14, Z15, Z16, Z17
  };

  enum class Subject : uint16_t {
    ACCT, ACN, ACTS, AERO, AHST, AMS, ARAB, ARHM, ARTS, ATCM, AUD, BA, BBSU, BCOM, BIOL, BIS, BLAW, BMEN, BPS, BUAN, CE, CGS, CHEM, CHIN, CLDP, COMD, COMM, CRIM, CRWT, CS, DANC, ECON, ECS, ECSC, ED, EE, EEBM, EECS, EECT, EEDG, EEGR, EEMF, EEOP, EEPE, EERF, EESC, ENGR, ENGY, ENTP, ENVR, EPCS, EPPS, FERM, FILM, FIN, FREN, GEOG, GEOS, GERM, GISC, GOVT, GST, HCS, HDCD, HIST, HLTH, HMGT, HONS, HUAS, HUHI, HUMA, HUSL, IMS, IPEC, ISAE, ISAH, ISIS, ISNS, ITSS, JAPN, LANG, LATS, LIT, MAIS, MAS, MATH, MECH, MECO, MILS, MIS, MKT, MSEN, MTHE, MUSI, NATS, NSC, OB, OBHR, OPRE, PA, PHIL, PHIN, PHYS, PPOL, PPPE, PSCI, PSY, PSYC, REAL, RHET, RMIS, SCI, SE, SMED, SOC, SPAN, SPAU, STAT, SYSE, SYSM, TE, THEA, UNIV, UTD, UTSW, VPAS
  };

  enum class Group : uint16_t {
    _9999, ARHM, ATEC, BBSC, ENCS, EPPS, GENS, HONS, MGMT, NSMT, UGRD
  };

  enum class Organization : uint16_t {
    ARHM, ATEC, BBSC, ENCS, ENCSBIEN, ENCSCSCI, ENCSELEC, ENCSMECH, ENCSMSEN, ENCSSYSM, EPPS, GENS, GENSOTED, HONS, MGMT, NSMT, NSMTBIOL, NSMTCHEM, NSMTGEOS, NSMTMATH, NSMTPHYS, NSMTSCED, REGS, UGRD, UTDAL
  };

  enum class Component : uint16_t {
    DSR, ENS, EXM, IND, INT, LAB, LBR, LE2, LEC, LLB, LLN, MST, PRA, RSC, RXM, SEM, STU
  };

  enum class Facility : uint16_t {
    NONE = 0, AB_1_308, AB_1_318, AB_1_506, AD_2_216, AD_2_232, AD_2_238, AH1_1_104, AH1_1_106, AH1_1_110, AH2_1_204, ATC_1_102, ATC_1_305, ATC_1_406, ATC_1_910, ATC_2_101, ATC_2_602, ATC_2_605, ATC_2_705E, ATC_2_908, ATC_2_914, ATC_2_918, ATC_3_205, ATC_3_502, ATC_3_601, ATC_3_904, ATC_3_910, ATC_3_914, ATC_4_902, ATC_4_906, ATC_4_910, BE_2_330, BE_2_506, BE_2_520, BSB14_102J, CB_1_102, CB_1_106, CB_1_202, CB_1_206, CB_1_210, CB_1_214, CB_1_218, CB_1_219, CB_1_222, CB_1_223, CB1_1_102, CB1_1_104, CB1_1_106, CB3_1_302, CB3_1_304, CB3_1_306, CB3_1_308, CB3_1_310, CB3_1_312, CB3_1_314, CBH_1_602, CD1_A_101, CD1_A_229, CD1_B_108, CD1_C_140, CD2_J_108, CD2_J_204, CD2_J_213, CHEC_106, CHEC_200, CHEC_222, CHEC_223, CR_1_202, CR_1_212, CR_1_508, CRA_11_205, CRA_12_110, CRA_12_120, CRA_12_125, ECSN_2_110, ECSN_2_112, ECSN_2_120, ECSN_2_126, ECSN_2_804, ECSN_3_108, ECSN_3_110, ECSN_3_112, ECSN_3_114, ECSN_3_118, ECSN_3_120, ECSN_4_324, ECSS_2_103, ECSS_2_201, ECSS_2_203, ECSS_2_305, ECSS_2_306, ECSS_2_311, ECSS_2_312, ECSS_2_410, ECSS_2_412, ECSS_2_415, ECSS_3_619, ECSS_3_910, ECSS_4_910, ECSW_1_130, ECSW_1_315, ECSW_1_355, ECSW_1_365, ECSW_2_315, ECSW_2_325, ECSW_2_335, ECSW_3_210, ECSW_3_250, ECSW_3_315, ECSW_3_325, ECSW_3_335, ECSW_4_315, ECSW_4_325, ECSW_4_335, FN_2_102, FN_2_104, FN_2_106, FN_2_202, FN_2_204, FN_2_210, FN_2_212, FN_2_214, FN_2_302, FN_2_304, FN_2_306, FN_3_410C, FO_1_202, FO_1_206C, FO_1_502, FO_2_208, FO_2_404, FO_2_608, FO_2_702, FO_3_206, FO_3_222, FO_3_616, GR_2_302, GR_2_530, GR_3_206, GR_3_302, GR_3_402A, GR_3_402B, GR_3_420, GR_3_602, GR_3_606, GR_4_204, GR_4_208, GR_4_301, GR_4_428, GR_4_708, GR_4_804, HH_2_402, HH_2_502, JO_1_216, JO_2_114, JO_2_504, JO_2_604, JO_3_209, JO_3_516, JO_3_532, JO_3_536, JO_3_906, JO_3_908, JO_4_102, JO_4_112, JO_4_502, JO_4_504, JO_4_614, JO_4_708, MC_2_410, MC_2_524, ML1_1_106, ML1_1_110, ML1_1_114, ML1_1_118, ML1_1_122, ML2_1_202, ML2_1_214, ML2_1_216, ML2_1_218, PHY_1_103, PHY_1_202, PHY_1_304, PS3_1_540, RHNW_1_001, RHNW_2_002, RHW_2_400, RHW_2_401, RHW_2_402, RHW_4_100, ROC_1_401, ROC_2_103, ROC_2_301T, ROC_2_701, ROW_1_141, SLC_1_102, SLC_1_202, SLC_1_204, SLC_1_206, SLC_1_211, SLC_1_214, SLC_2_202, SLC_2_203, SLC_2_206, SLC_2_207, SLC_2_215, SLC_2_216, SLC_2_302, SLC_2_303, SLC_2_304, SLC_3_102, SLC_3_202, SLC_3_203, SLC_3_210, SLC_3_215, SLC_3_220, SOM_1_102, SOM_1_107, SOM_1_110, SOM_1_117, SOM_1_118, SOM_1_211, SOM_1_212, SOM_1_217, SOM_1_302, SOM_1_502, SOM_1_508, SOM_1_516, SOM_11_202, SOM_11_206, SOM_11_210, SOM_12_202, SOM_12_206, SOM_12_210, SOM_12_214, SOM_12_218, SOM_12_222, SOM_13_501, SOM_13_510, SOM_14_501, SOM_2_102, SOM_2_103, SOM_2_106, SOM_2_107, SOM_2_112, SOM_2_115, SOM_2_116, SOM_2_117, SOM_2_402, SOM_2_714, SOM_2_717, SOM_2_722, SOM_2_801, SOM_2_802, SOM_2_803, SOM_2_804, SOM_2_901, SOM_2_902, SOM_2_903, SOM_2_904, SPN_1_115, SPN_1_121, SPN_1_221, SSA_13_330, SSB_3_107B, TH_2_702
  };

  enum class Mode : uint16_t {
    FACE_TO_FACE = 1, ONLINE = 2, HYBRID = 6
  };

  Section(std::shared_ptr<const std::vector<std::size_t>> students,
          std::size_t number,
          std::size_t course_id,
          Session session,
          time_t session_begin,
          Subject subject,
          std::string catalog_number,
          std::string section_name,
          Group group,
          Organization organization,
          std::size_t units,
          Component component,
          time_t start_date,
          time_t end_date,
          Facility facility,
          std::size_t room_capacity,
          time_t start_time,
          time_t end_time,
          bool monday,
          bool tuesday,
          bool wednesday,
          bool thursday,
          bool friday,
          bool saturday,
          bool sunday,
          Mode mode);

  virtual ~Section();

  virtual std::shared_ptr<const std::vector<std::size_t>> get_students() const final;

  virtual std::size_t get_number() const final;
  virtual std::size_t get_course_id() const final;
  virtual Session get_session() const final;
  virtual time_t get_session_begin() const final;
  virtual Subject get_subject() const final;
  virtual std::string get_catalog_number() const final;
  virtual std::string get_section_name() const final;
  virtual Group get_group() const final;
  virtual Organization get_organization() const final;
  virtual std::size_t get_units() const final;
  virtual Component get_component() const final;
  virtual time_t get_start_date() const final;
  virtual time_t get_end_date() const final;
  virtual Facility get_facility() const final;
  virtual std::size_t get_room_capacity() const final;
  virtual time_t get_start_time() const final;
  virtual time_t get_end_time() const final;
  virtual bool get_monday() const final;
  virtual bool get_tuesday() const final;
  virtual bool get_wednesday() const final;
  virtual bool get_thursday() const final;
  virtual bool get_friday() const final;
  virtual bool get_saturday() const final;
  virtual bool get_sunday() const final;
  virtual Mode get_mode() const final;

private:
  std::shared_ptr<const std::vector<std::size_t>> students;

  std::size_t number;
  std::size_t course_id;
  Session session;
  time_t session_begin;
  Subject subject;
  std::string catalog_number;
  std::string section_name;
  Group group;
  Organization organization;
  std::size_t units;
  Component component;
  time_t start_date;
  time_t end_date;
  Facility facility;
  std::size_t room_capacity;
  time_t start_time;
  time_t end_time;
  bool monday;
  bool tuesday;
  bool wednesday;
  bool thursday;
  bool friday;
  bool saturday;
  bool sunday;
  Mode mode;
};

}

bool operator==(const small_world::io::Section& l, const small_world::io::Section& r);
bool operator!=(const small_world::io::Section& l, const small_world::io::Section& r);
