#ifndef HELPERSINGLETON_H
#define HELPERSINGLETON_H

#include "cppimport_api.h"

class HelperSingleton
{
public:
    static HelperSingleton& getInstance();
    Model::Model* getModel(){ return model_;}
    OOModel::Project* getProject() { return project_;}
    void setProject(OOModel::Project* pro){project_ = pro; }
    void setModel(Model::Model* mod){ model_ = mod;}


private:
    HelperSingleton() {}
    Model::Model* model_;
    OOModel::Project* project_;
};

#endif // HELPERSINGLETON_H
