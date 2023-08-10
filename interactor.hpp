#ifndef INTERACTOR_H
#define INTERACTOR_H

#include "shader.hpp"
#include "model.hpp"

class Interactor {
    Model Copy;
    Model *Selector;
    int Selector_ID;
    Model *Reactor;
    int Reactor_ID;
    bool hasCopy = false;
    bool isMarkerDetected = false;
    bool selectorMarkerDetected = false;
    bool reactorMarkerDetected = false;

public:

    Interactor() {};
    Interactor(int selectorID, Model* &selector, int InteractorID, Model* &intaractor) {
        Selector = selector;
        Selector_ID = selectorID;
        Reactor = intaractor;
        Reactor_ID = InteractorID;
    }

    void set_selectorMarkerDetected(bool val) {
        selectorMarkerDetected = val;
        isMarkerDetected = val;
    }

    void set_reactorMarkerDetected(bool val) {
        reactorMarkerDetected = val;
        isMarkerDetected = val;
    }

    bool has_Copy() {
        return hasCopy;
    }

    bool isDetected() {
        return isMarkerDetected;
    }

    int get_selector_id() {
        return Selector_ID;
    }

    int get_reactor_id() {
        return Reactor_ID;
    }


    bool get_selectorMarkerDetected() {
        return selectorMarkerDetected;
    }

    bool get_reactorMarkerDetected() {
        return reactorMarkerDetected;
    }

    void set_modelCopy(Model* copy) {
        std::cout << "Modelo copiado\n";
        Copy = *copy;
        Copy.ViewMatrixavg = copy -> ViewMatrixavg;
        hasCopy = true;
    };

    Model get_model() {
        return Copy; 
    }

    void render(Shader &shader) {
        reactorMarkerDetected = false;

        if(!isMarkerDetected || !hasCopy) return;
        // selector detectado y modelo
        if(selectorMarkerDetected) {
            Copy.set_viewMatrix(Selector->get_viewMatrix());        
            Copy.render(shader);
            std::cout << "Renderizando Modelo\n";
        }
        isMarkerDetected = false;
        selectorMarkerDetected = false;
    }

    void set_hasCopy(bool val) {
        hasCopy = val;
    }

    glm::mat4 get_viewMatrix_selector(){
        return Selector->get_viewMatrix();
    }

    glm::mat4 get_viewMatrix_reactor(){
        return Reactor->View;
    }
        
private:

};

#endif