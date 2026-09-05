/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <bits/stdc++.h>
using namespace std;

class DocumentElement{
    public:
    virtual string render() = 0;
};

class TextElement : public DocumentElement {
private:
    string text;
public:
    TextElement(string text){
        this->text = text;
    }
    
    string render() override {
        return text;
    }
};

class ImageElement : public DocumentElement {
private:
    string imagePath;
public:
    ImageElement(string imagePath){
        this->imagePath = imagePath;
    }
    
    string render() override{
        return "Image: " + imagePath;
    }
};

class Document{
private:
    vector<DocumentElement*> documentElements;
    
public:
    void addElement(DocumentElement* element){
        documentElements.push_back(element);
    }
    
    string render(){
        string result;
        for(auto element : documentElements){
            result += element->render();
        }
        return result;
    }
};

class Persistance{
    public:
    virtual void save(string data) = 0;
};

class FileStorage : public Persistance {
public:
    void save(string data) override{
        ofstream outFile("document.txt");
        if(outFile){
            outFile<<data;
            outFile.close();
            cout<<"Document saved to document.txt"<<endl;
        }else{
            cout<<"Error: Unable to open file for writing."<<endl;
        }
    }
};

class DBStorage : public Persistance {
    public:
    void save(string data) override {
        cout<<"Business logic to run DB storage";
    }
};

class DocumentEditor{
    private:
    Document* document;
    Persistance* storage;
    string renderedDocument;
    
public:
    DocumentEditor(Document* document, Persistance* storage){
        this->document = document;
        this->storage = storage;
    }
    
    void addText(string text){
        document->addElement(new TextElement(text));
        
    }
    
    void addImage(string imagePath){
        document->addElement(new ImageElement(imagePath));
    }
    
    string renderDocument(){
        if(renderedDocument.empty()){
            renderedDocument = document->render();
        }
        return renderedDocument;
    }
    
    void saveDocument(){
        storage->save(renderDocument());
    }
};



int main()
{
    std::cout<<"Hello World";
    Document* document = new Document();
    Persistance* persistance = new FileStorage();
    
    DocumentEditor* editor = new DocumentEditor(document, persistance);
    
    editor->addText("Hello Sanu");
    editor->addImage("Hello/sanu/tmp");
    cout<<editor->renderDocument();
    
    
    return 0;
}
