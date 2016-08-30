#include "controller.h"

Controller::Controller(Depends *deps, QString deptype) : deps_(deps)
{
    GetDepends *gdeps = new GetDepends(*deps_);
    gdeps->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, gdeps, &QObject::deleteLater);
    if (deptype == "Depends")
    {
        connect(this, &Controller::operate, gdeps, &GetDepends::get_depends);
        connect(gdeps, &GetDepends::dependsReady, deps_, &Depends::updateDeps);
        connect(gdeps, &GetDepends::invalidPackage, deps_, &Depends::depsInvalid);
    }
    else if (deptype == "Recommends")
    {
        connect(this, &Controller::operate, gdeps, &GetDepends::get_recs);
        connect(gdeps, &GetDepends::recommendsReady, deps_, &Depends::updateRecs);
        connect(gdeps, &GetDepends::invalidPackage, deps_, &Depends::recsInvalid);
    }
    if (deptype == "ParentsDepends")
    {
        connect(this, &Controller::operate, gdeps, &GetDepends::get_parents_depends);
        connect(deps_, &Depends::parentsDependsReady, deps_, &Depends::updateParsDeps);
        connect(gdeps, &GetDepends::invalidPackage, deps_, &Depends::parsDepsInvalid);
    }
    if (deptype == "ParentsRecommends")
    {
        connect(this, &Controller::operate, gdeps, &GetDepends::get_parents_recommends);
        connect(deps_, &Depends::parentsRecommendsReady, deps_, &Depends::updateParsRecs);
        connect(gdeps, &GetDepends::invalidPackage, deps_, &Depends::parsRecsInvalid);
    }
    else if (deptype == "policy")
    {
        connect(this, &Controller::operate, gdeps, &GetDepends::get_pol);
        connect(gdeps, &GetDepends::policyReady, deps_, &Depends::updatePolicy);
        connect(gdeps, &GetDepends::polDone, deps_, &Depends::policyDone);
    }
    connect(gdeps, &GetDepends::invalidPackage, deps_, &Depends::badPackage);

    workerThread.start();
}

Controller::~Controller() {
    workerThread.quit();
    workerThread.wait();
}
