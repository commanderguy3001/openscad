#include "parameterset.h"
#include "modcontext.h"
#include "comment.h"

ParameterSet::ParameterSet()
{
}
ParameterSet::~ParameterSet()
{

}

void ParameterSet::getParameterSet(const std::string &filename)
{
    std::fstream myfile;
    myfile.open (filename);
    // send your JSON above to the parser below, but populate ss first
    if(myfile.is_open()){
        try{
            pt::read_json(myfile, this->root);
        }
        catch (std::exception const& e){

            std::cerr << e.what() << std::endl;
        }
    }
     myfile.close();
}

void ParameterSet::writeParameterSet(const std::string &filename){
    if(root.empty()){
        return;
    }

    std::fstream myfile;
    myfile.open(filename, std::ios::out);
    // send your JSON above to the parser below, but populate ss first
    if(myfile.is_open()){
        try{
            pt::write_json(myfile, this->root);
        }
        catch (std::exception const& e){

            std::cerr << e.what() << std::endl;
        }
    }
     myfile.close();
}

void ParameterSet::applyParameterSet(FileModule *fileModule, const std::string &setName)
{
    try{
        if (fileModule == NULL ||root.empty()) {
            return;
        }
        ModuleContext ctx;
        std::string path="SET."+setName;
        for (AssignmentList::iterator it = fileModule->scope.assignments.begin();it != fileModule->scope.assignments.end();it++) {

            for(pt::ptree::value_type &v : root.get_child(path)){
                if(v.first== (*it).name){
                    Assignment *assignment;
                    assignment=&(*it);
                    const ValuePtr defaultValue = assignment->expr.get()->evaluate(&ctx);
                    if(defaultValue->type()== Value::STRING){

                        assignment->expr = shared_ptr<Expression>(new Literal(ValuePtr(v.second.data())));
                    }else if(defaultValue->type()== Value::BOOL){

                       assignment->expr = shared_ptr<Expression>(new Literal(ValuePtr(v.second.get_value<bool>())));
                    }else{

                      shared_ptr<Expression> params = CommentParser::parser(v.second.data().c_str());
                      if (!params) continue;
                        ModuleContext ctx;
                        if (defaultValue->type() == params->evaluate(&ctx)->type()) {
                          assignment->expr = params;
                        }
                    }
                }
             }
        }
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

