/***************************************************************************
begin                : Mon Nov 3 15:54:58 CEST 2014
copyright            : (C) 2014 Simoncelli Stefano
email                : simoncelli.stefano@hotmail.it
***************************************************************************/

#include <rtsim/AVRTask.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

namespace RTSim {

    using namespace MetaSim;

    using std::unique_ptr;
    using std::vector;

    AVRTask::~AVRTask() {
        // std::cout << "Inside delete" << std::endl;

        /// PEZZO DI CODICE INCRIMINATO

        // if (myInstr.size() > 0){
        // 	auto s = myInstr.begin();
        // 	while (s != myInstr.end()){
        // 		auto j = s->begin();
        // 		while (j != s->end()) {
        // 			delete (*j);
        // 			j++;
        // 		}
        // 		s++;
        // 	}
        // }

        myInstr.clear();
        BufferedDeadlines.clear();
        BufferedModes.clear();
        // std::cout << "delete completed" << std::endl;
    }

    void AVRTask::buildInstr(const vector<string> &param) { // throw(ParseExc) {
        /// PEZZO DI CODICE INCRIMINATO
        // if (myInstr.size() > 0){
        // 	vector<vector<Instr*>>::iterator s = myInstr.begin();
        // 	while (s != myInstr.end()){
        // 		vector < Instr* > ::iterator j = (*s).begin();
        // 		while (j != (*s).end()){
        // 			delete (*j);
        // 			j++;
        // 		}
        // 		s++;
        // 	}
        // }

        myInstr.clear();

        for (unsigned int k = 0; k < param.size(); k++) {
            vector<string> instrAtMode = split_instr(param.at(k));
            vector<RTSim::Instr *> myCurrInstr;

            for (unsigned int i = 0; i < instrAtMode.size(); ++i) {
                string token = get_token(instrAtMode[i]);
                string param = get_param(instrAtMode[i]);

                vector<string> par_list = split_param(param);

                par_list.push_back(string(getName()));

                unique_ptr<RTSim::Instr> curr =
                    genericFactory<RTSim::Instr>::instance().create(token,
                                                                    par_list);

                if (!curr)
                    throw ParseExc("insertCode", token);

                RTSim::Instr *curr_instr = curr.release();
                myCurrInstr.push_back(curr_instr);
            }

            myInstr.push_back(myCurrInstr);
        }

        // std::cout << "Creation terminated" << std::endl;
    }

    AVRTask::AVRTask(double angPeriod, double angPhase, double angDl,
                     const vector<string> &instr,
                     const vector<double> &Omegaplus,
                     const vector<double> &Omegaminus,
                     const std::string &name) :
        // throw(WrongParameterSize) :
        Task(NULL, 0, 0, name, 1000),
        AngularPeriod(angPeriod),
        AngularPhase(angPhase),
        AngularDl(angDl) {
        if (instr.size() != Omegaminus.size() ||
            instr.size() != Omegaplus.size())
            throw WrongParameterSize(
                "instruction, OmegaPlus and OmegaMinus sizes must be all equal "
                "to the max mode number");

        buildInstr(instr);

        OmegaPlus.assign(Omegaplus.begin(), Omegaplus.end());
        OmegaMinus.assign(Omegaminus.begin(), Omegaminus.end());
        std::transform(OmegaPlus.begin(), OmegaPlus.end(), OmegaPlus.begin(),
                       std::bind1st(std::multiplies<double>(), 2 * M_PI / 60));
        std::transform(OmegaMinus.begin(), OmegaMinus.end(), OmegaMinus.begin(),
                       std::bind1st(std::multiplies<double>(), 2 * M_PI / 60));
    }

    // redefined because in the Task::newRun() check for Empty Instruction Queue
    // is performed AVRTasks update their instruction queue at runtime only
    // after they are activated (there are different possibles instruction
    // queues) NO CHECK FOR EMPTY INSTR QUEUE IN NEW RUN
    void AVRTask::newRun(void) {
        state = TSK_IDLE;
        while (chkBuffArrival())
            unbuffArrival();

        lastArrival = arrival = phase;
        if (int_time != NULL)
            arrEvt.post(arrival);
        _dl = 0;
        BufferedDeadlines.clear();
        BufferedModes.clear();
    }

    // not to be called when task is active
    // only when a job is finished and before activating the next one
    void AVRTask::changeStatus(
        double angper, double angphase, double angdl,
        const vector<string> &instr, const vector<double> &OmegaM,
        const vector<double> &OmegaP) { // throw(TaskAlreadyActive) {
        if (instr.size() != OmegaM.size() || instr.size() != OmegaP.size())
            throw WrongParameterSize(
                "instruction, OmegaPlus and OmegaMinus sizes must be all equal "
                "to the max mode number");
        if (isActive())
            throw TaskAlreadyActive();

        AngularPeriod = angper;
        AngularPhase = angphase;
        AngularDl = angdl;

        buildInstr(instr);

        OmegaPlus.assign(OmegaP.begin(), OmegaP.end());
        OmegaMinus.assign(OmegaM.begin(), OmegaM.end());
        std::transform(OmegaPlus.begin(), OmegaPlus.end(), OmegaPlus.begin(),
                       std::bind1st(std::multiplies<double>(), 2 * M_PI / 60));
        std::transform(OmegaMinus.begin(), OmegaMinus.end(), OmegaMinus.begin(),
                       std::bind1st(std::multiplies<double>(), 2 * M_PI / 60));
    }

    void AVRTask::handleArrival(Tick arr) {
        setRelDline(*(BufferedDeadlines.begin()));
        BufferedDeadlines.erase(BufferedDeadlines.begin());

        mode = *(BufferedModes.begin());
        BufferedModes.erase(BufferedModes.begin());

        // std::cout << "AVRTask::handleArrival at " << arr << " BEFORE" <<
        // std::endl;
        instrQueue.clear();
        // std::cout << "AVRTask::handleArrival at " << arr << " AFTER" <<
        // std::endl;

        // if (instrQueue.begin() != instrQueue.end()) std::cout << "SOMETHING
        // REALLY WRONG" << std::endl;

        vector<RTSim::Instr *> myCurrInstr = myInstr.at(mode);

        vector<RTSim::Instr *>::iterator j = myCurrInstr.begin();
        while (j != myCurrInstr.end()) {
            /// PEZZO INCRIMINATO: bisogna prima clonare l'oggetto, e poi darlo
            /// in pasto allo unique_ptr
            addInstr(unique_ptr<Instr>(*j));
            j++;
        }

        // std::cout << "AVRTask::handleArrival at " << arr << " before
        // TASK::handleArrival" << std::endl;

        Task::handleArrival(arr);

        // std::cout << "AVRTask::handleArrival at " << arr << " after
        // TASK::handleArrival" << std::endl;
    }

    void AVRTask::activate(int mode, Tick rdl) { // throw(ModeOutOfIndex) {
        if (mode >= myInstr.size() || mode < 0)
            throw ModeOutOfIndex("Mode Index out of range");
        this->BufferedDeadlines.push_back(rdl);
        this->BufferedModes.push_back(mode);
        arrEvt.drop();
        arrEvt.post(SIMUL.getTime());
    }

    Tick AVRTask::getWCET(int index) const { // throw(ModeOutOfIndex) {
        if (index >= myInstr.size() || index < 0)
            throw ModeOutOfIndex("Mode Index out of range");
        Tick tt = 0;

        vector<RTSim::Instr *> myCurrInstr = myInstr.at(index);
        auto i = myCurrInstr.begin();
        while (i != myCurrInstr.end()) {
            tt += (*i)->getWCET();
            i++;
        }

        return tt;
    }

    unique_ptr<AVRTask> AVRTask::createInstance(const vector<string> &par) {
        double angPer = atof(par[0].c_str());
        double angPhase = atof(par[1].c_str());
        double angDl = atof(par[2].c_str());

        int Nmodes = par.size() - 5;

        vector<string> instr;
        int i = 0;
        while (i < Nmodes) {
            string prova = par[3 + i];
            instr.push_back(prova);
            i++;
        }

        vector<double> omegaPlus;
        size_t prev_pos = 0;
        size_t pos = par[3 + i].find(",", 0);
        while (pos != string::npos) {
            pos = par[3 + i].find(",", prev_pos);
            double prova = atof(par[3 + i].substr(prev_pos, pos).c_str());
            prev_pos = pos + 1;
            omegaPlus.push_back(prova);
        }

        vector<double> omegaMinus;
        prev_pos = 0;
        pos = par[4 + i].find(",", 0);
        while (pos != string::npos) {
            pos = par[4 + i].find(",", prev_pos);
            double prova = atof(par[4 + i].substr(prev_pos, pos).c_str());
            prev_pos = pos + 1;
            omegaMinus.push_back(prova);
        }

        string n = "";
        if (5 + i < par.size())
            n = par[5 + i];

        return unique_ptr<AVRTask>(new AVRTask(angPer, angPhase, angDl, instr,
                                               omegaPlus, omegaMinus, n));
    }
} // namespace RTSim
