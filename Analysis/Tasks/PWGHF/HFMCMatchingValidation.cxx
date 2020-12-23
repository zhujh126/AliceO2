
// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file HFMCMAtchingValidation.cxx
/// \brief validation code -- D0 candidates 


#include "Framework/AnalysisTask.h"
#include "Framework/HistogramRegistry.h"
#include "Framework/runDataProcessing.h"
#include "AnalysisCore/RecoDecay.h"
#include "AnalysisDataModel/HFSecondaryVertex.h"
#include "AnalysisDataModel/HFCandidateSelectionTables.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::aod::hf_cand_prong2;
using namespace o2::aod::hf_cand_prong3;
using namespace o2::framework::expressions;

//Analysis Task done to look at the MC truth
struct TaskQQbarMCgen {
  HistogramRegistry registry{
    "registry",
    {{"hMCEvt_pTDiffMotherDaughterGen", "Pt DIfference Mother-Daughters", {HistType::kTH1F, {{ 300, -5., +1.}}}},
     {"hMCEvt_countAverageC", "Event counter - Average Number Charm quark; entries", {HistType::kTH1F, {{ 20, -0., +20.}}}},
     {"hMCEvt_countAverageB", "Event counter - Average Number Beauty quark; entries", {HistType::kTH1F, {{ 20, -0., +20.}}}},
     {"hMCEvt_countAverageCbar", "Event counter - Average Number Anti-Charm quark; entries", {HistType::kTH1F, {{ 20, -0., +20.}}}},
     {"hMCEvt_countAverageBbar", "Event counter - Average Number Anti-Beauty quark; entries", {HistType::kTH1F, {{ 20, -0., +20.}}}},
     {"hMCEta_quarks", "Eta quarks;candidate #it{#eta};entries", {HistType::kTH1F, {{250, -5., 5.}}}},
     {"hMCEvt_matchD0ToPiK_tot", "Event counter - D0ToPiK - tot; entries", {HistType::kTH1F, {{ 10, -0.5, +9.5}}}},
     {"hMCEvt_matchD0ToPiK_atCollVert", "Event counter - D0ToPiK - atCollVert; entries", {HistType::kTH1F, {{ 10, -0.5, +9.5}}}},
     {"hMCEvt_matchD0ToPiK_Cdecays", "Event counter - D0ToPiK - Cdecays; entries", {HistType::kTH1F, {{ 10, -0.5, +9.5}}}},
     {"hMCEvt_matchD0ToPiK_Bdecays", "Event counter - D0ToPiK - Bdecays; entries", {HistType::kTH1F, {{ 10, -0.5, +9.5}}}},
     {"hMCEvt_pTmotherVsSumpTdaughters", "entries - SumPtDaughters(y) VS PtMother (x)", {HistType::kTH2F, {{300, 0, 30. }, { 300, -0., 30.}}}}}};
 
  //Configurable<double> cutEtaCandMax{"cutEtaCandMax", -1., "max. cand. pseudorapidity"};
  //Configurable<double> cutPtCandMin{"cutPtCandMin", -1., "min. cand. pT"};

  void process(aod::McCollision const& mccollision, soa::Join<aod::McParticles, aod::HfCandProng2MCGen> const& particlesMC){//,  soa::Join<aod::McParticles,aod::HfCandProng3MCGen> const& particles3MC){

    int cpercollision = 0;     int cbarpercollision = 0;
    int bpercollision = 0;    int bbarpercollision = 0;
    int matchD0ToPiK_tot=0;  
    int matchD0ToPiK_atCollVert=0;    
    int matchD0ToPiK_Cdecays=0;
    int matchD0ToPiK_Bdecays=0;


    
    Configurable<int> d_selectionFlagD0{"d_selectionFlagD0", 0, "Selection Flag for D0"};

    //counting HF quarks
    std::cout << "New MC Collision"<< std::endl;
    //std::cout << mccollision.generatorsID() <<  std::endl;  //Checking the variation of the collision index
    for (auto& particle1 : particlesMC) {
      //if (cutEtaCandMax >= 0. && std::abs(particle1.eta()) > cutEtaCandMax) continue;
      //if (cutPtCandMin >= 0. && std::abs(particle1.pt()) < cutPtCandMin) continue;

      if (particle1.pdgCode() == 4) {
        if(particlesMC.iteratorAt(particle1.mother0()).pdgCode()!=4) {
          cpercollision++;
        }
      }//end charm check
      if (particle1.pdgCode() == -4) {
        if(particlesMC.iteratorAt(particle1.mother0()).pdgCode()!=-4) {
          cbarpercollision++;
          //std::cout << "Candidates for this event are: " << cbarpercollision << std::endl;
        }
      }//end cbar check
      if (particle1.pdgCode() == 5) {
        if(particlesMC.iteratorAt(particle1.mother0()).pdgCode()!=5) {
          bpercollision++;
        }
      }//end beauty check
      if (particle1.pdgCode() == -5) {
        if(particlesMC.iteratorAt(particle1.mother0()).pdgCode()!=-5) {
          bbarpercollision++;
        }
      }//end beauty check

      if (std::abs(particle1.pdgCode()) == 4 || std::abs(particle1.pdgCode()) == 5) {
        if(std::abs(particlesMC.iteratorAt(particle1.mother0()).pdgCode())!=5 && std::abs(particlesMC.iteratorAt(particle1.mother0()).pdgCode())!=4){
          registry.get<TH1>("hMCEta_quarks")->Fill(particle1.eta());
        }
      }

      //Matching candidates D0s and checking their origin
      if (std::abs(particle1.flagMCMatchGen()) == D0ToPiK) {
        matchD0ToPiK_tot++;
        registry.get<TH1>("hMCEvt_pTDiffMotherDaughterGen")->Fill(particle1.pt()-particlesMC.iteratorAt(particle1.daughter0()).pt()-particlesMC.iteratorAt(particle1.mother0()).pt());
        registry.get<TH2>("hMCEvt_pTmotherVsSumpTdaughters")->Fill(particle1.pt(),particlesMC.iteratorAt(particle1.daughter0()).pt()+particlesMC.iteratorAt(particle1.mother0()).pt());
        //directly produced at the interaction vertex
        if(std::abs(particlesMC.iteratorAt(particle1.mother0()).pdgCode()) == 4){
          matchD0ToPiK_atCollVert++;
          std::cout << "From C at coll vertex" << "     Mother pDG code: " << particlesMC.iteratorAt(particle1.mother0()).pdgCode() << std::endl;
          std::cout << "Collision : PosX() = "<<  mccollision.posX() << "  D0  Particle : Vx() ="  << particle1.vx() << std::endl;
          std::cout << "                     "<<  mccollision.posX() << "  Dau0  Parti  : Vx() ="  << particlesMC.iteratorAt(particle1.daughter0()).vx() << std::endl;
          std::cout << "                     "<<  mccollision.posX() << "  Dau1  Parti  : Vx() ="  << particlesMC.iteratorAt(particle1.daughter1()).vx() << std::endl;

        }
        if((particlesMC.iteratorAt(particle1.mother0()).pdgCode()>400 && particlesMC.iteratorAt(particle1.mother0()).pdgCode()<500) || (particlesMC.iteratorAt(particle1.mother0()).pdgCode()>4000 && particlesMC.iteratorAt(particle1.mother0()).pdgCode()<5000)){
          matchD0ToPiK_Cdecays++;  
          std::cout << "From Charmed Mesons Decays)" << "     Mother pDG code: " << particlesMC.iteratorAt(particle1.mother0()).pdgCode() << std::endl;
          std::cout << "Collision : PosX() = "<<  mccollision.posX() << "  D0  Particle : Vx() ="  << particle1.vx() << std::endl;
          std::cout << "                     "<<  mccollision.posX() << "  Dau0  Parti  : Vx() ="  << particlesMC.iteratorAt(particle1.daughter0()).vx() << std::endl;
          std::cout << "                     "<<  mccollision.posX() << "  Dau1  Parti  : Vx() ="  << particlesMC.iteratorAt(particle1.daughter1()).vx() << std::endl;

        }
        if((particlesMC.iteratorAt(particle1.mother0()).pdgCode()>500 && particlesMC.iteratorAt(particle1.mother0()).pdgCode()<600) || (particlesMC.iteratorAt(particle1.mother0()).pdgCode()>5000 && particlesMC.iteratorAt(particle1.mother0()).pdgCode()<6000)){
          matchD0ToPiK_Bdecays++;
          std::cout << "From Beauty Mesons Decays)" << "     Mother pDG code: " << particlesMC.iteratorAt(particle1.mother0()).pdgCode() << std::endl;
          std::cout << "Collision : PosX() = "<<  mccollision.posX() << "  D0  Particle : Vx() ="  << particle1.vx() << std::endl;
          std::cout << "                     "<<  mccollision.posX() << "  Dau0  Parti  : Vx() ="  << particlesMC.iteratorAt(particle1.daughter0()).vx() << std::endl;
          std::cout << "                     "<<  mccollision.posX() << "  Dau1  Parti  : Vx() ="  << particlesMC.iteratorAt(particle1.daughter1()).vx() << std::endl;

        }
      }

    }//end particles



    registry.get<TH1>("hMCEvt_countAverageC")->Fill(cpercollision);
    registry.get<TH1>("hMCEvt_countAverageB")->Fill(bpercollision);
    registry.get<TH1>("hMCEvt_countAverageCbar")->Fill(cbarpercollision);
    registry.get<TH1>("hMCEvt_countAverageBbar")->Fill(bbarpercollision);
    registry.get<TH1>("hMCEvt_matchD0ToPiK_tot")->Fill(matchD0ToPiK_tot);
    registry.get<TH1>("hMCEvt_matchD0ToPiK_atCollVert")->Fill(matchD0ToPiK_atCollVert);
    registry.get<TH1>("hMCEvt_matchD0ToPiK_Cdecays")->Fill(matchD0ToPiK_Cdecays);
    registry.get<TH1>("hMCEvt_matchD0ToPiK_Bdecays")->Fill(matchD0ToPiK_Bdecays);

  }
};

struct TaskAverage {
  HistogramRegistry registry{
    "registry",
     {{"hMCEvt_countRecoD0ToPiK", "Event counter - D0ToPiK; entries", {HistType::kTH1F, {{ 15, -0, +15}}}},
      {"hist_Coll", "Collision Size", {HistType::kTH1F, {{ 1000, -0, +1000}}}},
      {"hist_Average_D0", "#D0 per collision", {HistType::kTH1F, {{ 1000, -0, +1}}}}}};

  Configurable<int> d_selectionFlagD0{"d_selectionFlagD0", 0, "Selection Flag for D0"};
  Configurable<int> d_selectionFlagD0bar{"d_selectionFlagD0bar", 0, "Selection Flag for D0bar"};
  Configurable<double> cutEtaCandMax{"cutEtaCandMax", -1., "max. cand. pseudorapidity"};

  Filter filterSelectCandidates = (aod::hf_selcandidate_d0::isSelD0 >= d_selectionFlagD0 || aod::hf_selcandidate_d0::isSelD0bar >= d_selectionFlagD0bar);

  void process(aod::Collisions const& collisions, soa::Filtered<soa::Join<aod::HfCandProng2, aod::HFSelD0Candidate, aod::HfCandProng2MCRec>> const& candidates)
  {

    int recoD0ToPiKCandidates=0;
    int collision=0;
    collision += collisions.size();
    registry.get<TH1>("hist_Coll")->Fill(collisions.size());

    std::cout << "check " << collisions.size() <<  std::endl;
    for(auto& candidate : candidates){
      /*if (cutEtaCandMax >= 0. && std::abs(candidate.eta()) > cutEtaCandMax) {
        //Printf("Candidate: eta rejection: %g", candidate.eta());
        continue;
      }*/
      if (candidate.flagMCMatchRec() == D0ToPiK) {
        recoD0ToPiKCandidates++;
      }
    }//end loop on candidates
    
    //Filling Histograms
    registry.get<TH1>("hMCEvt_countRecoD0ToPiK")->Fill(recoD0ToPiKCandidates);
    double AverageD0perCollision = (double)recoD0ToPiKCandidates/(double)collisions.size();
    registry.get<TH1>("hist_Average_D0")->Fill(AverageD0perCollision);

  } //end process
};

//Comparison task between reco and gen level

#include "AnalysisCore/RecoDecay.h"

struct TaskCompareRecTruth {
  HistogramRegistry registry{
    "registry",
     {{"hist_pt", "PT difference reco with MC", {HistType::kTH1F, {{ 200, -1, +1}}}},
      {"hist_SecVx", "Secondary Vertex difference reco with MC true", {HistType::kTH1F, {{ 200, -1, +1}}}},
      {"hist_SecVy", "Secondary Vertex difference reco with MC true", {HistType::kTH1F, {{ 200, -1, +1}}}},
      {"hist_SecVz", "Secondary Vertex difference reco with MC true", {HistType::kTH1F, {{ 200, -1, +1}}}}}};

  Configurable<int> d_selectionFlagD0{"d_selectionFlagD0", 0, "Selection Flag for D0"};
  Configurable<int> d_selectionFlagD0bar{"d_selectionFlagD0bar", 0, "Selection Flag for D0bar"};
  Configurable<double> cutEtaCandMax{"cutEtaCandMax", -1., "max. cand. pseudorapidity"};

  Filter filterSelectCandidates = (aod::hf_selcandidate_d0::isSelD0 >= d_selectionFlagD0 || aod::hf_selcandidate_d0::isSelD0bar >= d_selectionFlagD0bar);

  void process(soa::Filtered<soa::Join<aod::HfCandProng2, aod::HFSelD0Candidate, aod::HfCandProng2MCRec>> const& candidates, aod::BigTracksMC const& tracks, aod::McParticles const& particlesMC)
  {

    int recoD0ToPiKCandidates=0;
    int collisioni=0;
    int indexParticle=0;

    for(auto& candidate : candidates){
      /*if (cutEtaCandMax >= 0. && std::abs(candidate.eta()) > cutEtaCandMax) {
        //Printf("Candidate: eta rejection: %g", candidate.eta());
        continue;
      }*/
      if (candidate.flagMCMatchRec() == D0ToPiK) {
        recoD0ToPiKCandidates++;
        indexParticle = RecoDecay::getMother(particlesMC, candidate.index0_as<aod::BigTracksMC>().label(), 421);
        //std::cout << "Checking mother particle MC truth" <<  indexParticle << std::endl;
        registry.get<TH1>("hist_pt")->Fill(candidate.pt()-particlesMC.iteratorAt(indexParticle).pt());
        //particleMC.vz() stands for the production coordinate of the particle
        registry.get<TH1>("hist_SecVx")->Fill(candidate.xSecondaryVertex()-particlesMC.iteratorAt(particlesMC.iteratorAt(indexParticle).daughter0()).vx());
        registry.get<TH1>("hist_SecVy")->Fill(candidate.ySecondaryVertex()-particlesMC.iteratorAt(particlesMC.iteratorAt(indexParticle).daughter0()).vy());
        registry.get<TH1>("hist_SecVz")->Fill(candidate.zSecondaryVertex()-particlesMC.iteratorAt(particlesMC.iteratorAt(indexParticle).daughter0()).vz());


      }
    }//end loop on candidates
    

  } //end process
};



WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  WorkflowSpec workflow{
  	adaptAnalysisTask<TaskQQbarMCgen>("hf-task-qqbar-mc-gen"),
  	adaptAnalysisTask<TaskAverage>("hf-task-average"),
  	adaptAnalysisTask<TaskCompareRecTruth>("hf-task-compare")};



  return workflow;
}




