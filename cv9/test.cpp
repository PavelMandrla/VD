//
// Created by pavel on 15.11.21.
//

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>

int frameI = 0;
vtkActor * actor;
vtkRenderWindow * renWin;
vtkRenderWindowInteractor * renderWindowInteractor;
vtkXMLPolyDataReader * reader;

void readFile(int i) {
    std::string filename = std::to_string(i) + ".vtp";

    reader->SetFileName(filename.c_str());
    reader->Update();
}

class LoadNextCommand : public vtkCommand {
public:
vtkTypeMacro(LoadNextCommand, vtkCommand);

    static LoadNextCommand * New() {
        return new LoadNextCommand;
    }

    void Execute(vtkObject * vtkNotUsed(caller), unsigned long vtkNotUsed(eventId), void * vtkNotUsed(callData)) {
        frameI++;
        if (frameI == 386) frameI = 0;
        readFile(frameI);
        renWin->Render();
    }
};


int main(int argc, char* argv[]) {
    auto colors = vtkNamedColors::New();
    auto renderer = vtkRenderer::New();

    // Create render window
    renWin = vtkRenderWindow::New();
    renWin->AddRenderer(renderer);
    renWin->SetSize(600,600);

    reader = vtkXMLPolyDataReader::New();
    readFile(frameI);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(reader->GetOutputPort());

    actor = vtkActor::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("NavajoWhite").GetData());

    renderer->AddActor(actor);
    renderer->ResetCamera();

    // Create a RenderWindowInteractor
    renderWindowInteractor = vtkRenderWindowInteractor::New();
    renderWindowInteractor->SetRenderWindow(renWin);
    renderWindowInteractor->Initialize();
    renderWindowInteractor->CreateRepeatingTimer(1);
    LoadNextCommand * loadCallback =  LoadNextCommand::New();
    renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, loadCallback );

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
