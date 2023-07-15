#ifndef INTERACTOR_H
#define INTERACTOR_H

#include "shader.hpp"
#include "model.hpp"

class Interactor {
    Model Copy;
    Model *Selector;
    int Selector_ID;
    bool hasCopy = false;
    bool selectorMarkerDetected = false;
    bool isMarkerDetected = false;

public:
    Interactor(int selectorID, Model* &selector) {
        Selector = selector;
        Selector_ID = selectorID;
    }

    void set_selectorMarkerDetected(bool val) {
        selectorMarkerDetected = val;
        isMarkerDetected = val;
    }

    bool isDetected() {
        return isMarkerDetected;
    }

    int get_selector_id() {
        return Selector_ID;
    }

    void set_modelCopy(Model* copy) {
        Copy = *copy;
        hasCopy = true;
    };

    void render(Shader &shader) {
        if(!isMarkerDetected || !hasCopy) return;
        Copy.set_viewMatrix(Selector->get_viewMatrix());
        Copy.render(shader);

        isMarkerDetected = false;
    }

    glm::mat4 get_viewMatrix_selector(){
        return Selector->get_viewMatrix();
    }
        
private:

};

#endif