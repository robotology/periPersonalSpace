#include "virtContactGenThread.h"
#include <yarp/sig/Image.h>

//see also Compensator::setTaxelPosesFromFile in icub-main/src/modules/skinManager/src/compensator.cpp
//see also dbool vtRFThread::setTaxelPosesFromFile(const string filePath, skinPartPWE &sP)
int virtContactGenerationThread::initSkinParts()
{
    SkinPart skin_part_name; 
       
    string line;
    ifstream posFile;
    yarp::sig::Vector taxelPos(3,0.0);
    yarp::sig::Vector taxelNorm(3,0.0);
   
    string filename; 
    //go through skin parts and initialize them
    for (std::vector<SkinPart>::const_iterator it = activeSkinPartsNames.begin() ; it != activeSkinPartsNames.end(); ++it){
        skin_part_name = *it;
        skinPartTaxel skinPartWithTaxels; 
        
        // Open File
        posFile.open(skinPartPosFilePaths[skin_part_name].c_str());  
        if (!posFile.is_open())
        {
           yWarning("[virtContactGenerationThread] File %s has not been opened!",skinPartPosFilePaths[skin_part_name].c_str());
           return false;
        }
        printMessage(4,"Initializing %s from %s.\n",SkinPart_s[skin_part_name].c_str(),skinPartPosFilePaths[skin_part_name].c_str());
        posFile.clear(); 
        posFile.seekg(0, std::ios::beg);//rewind iterator
        
        switch(skin_part_name){
            case SKIN_LEFT_HAND:
            case SKIN_RIGHT_HAND:
                for(unsigned int i= 0; getline(posFile,line); i++)
                {
                    line.erase(line.find_last_not_of(" \n\r\t")+1);
                    if(line.empty())
                        continue;
                    string number;
                    istringstream iss(line, istringstream::in);
                    taxelPos.zero(); taxelNorm.zero();
                    for(unsigned int j = 0; iss >> number; j++ )
                    {
                        if(j<3)
                            taxelPos[j] = strtod(number.c_str(),NULL);
                        else
                            taxelNorm[j-3] = strtod(number.c_str(),NULL);
                    }
                    skinPartWithTaxels.size++; //this is incremented for all lines - size of "port"
                    if((i>=96) && (i<=143) && (i!=107) && (i!=119) && (i!=131) && (i!=139)) //all palm taxels, without thermal pads
                    {
                        skinPartWithTaxels.txls.push_back(new Taxel(taxelPos,taxelNorm,i));
                        printMessage(10,"Pushing taxel ID:%d, pos:%f %f %f; norm:%f %f %f.\n",i,taxelPos[0],taxelPos[1],taxelPos[2],taxelNorm[0],taxelNorm[1],taxelNorm[2]);
                    }
                }
                if(skinPartWithTaxels.size != 192){
                    yWarning("[virtContactGenerationThread]::initSkinParts():initalizing %s from file, 192 positions expected, but %d present.\n",SkinPart_s[skin_part_name].c_str(),skinPartWithTaxels.size);
                }
                skinPartWithTaxels.name = skin_part_name;
                if (skin_part_name == SKIN_LEFT_HAND){
                   activeSkinParts[SKIN_LEFT_HAND] = skinPartWithTaxels;
                   printMessage(4,"Adding SKIN_LEFT_HAND to activeSkinParts, it now has %d members.\n",activeSkinParts.size());
                }
                else{  // skin_part_name == SKIN_RIGHT_HAND
                    activeSkinParts[SKIN_RIGHT_HAND] = skinPartWithTaxels;
                    printMessage(4,"Adding SKIN_RIGHT_HAND to activeSkinParts, it now has %d members.\n",activeSkinParts.size());
                }
                break;
                
            case SKIN_LEFT_FOREARM:
            case SKIN_RIGHT_FOREARM:
                for(unsigned int i= 0; getline(posFile,line); i++)
                {
                    line.erase(line.find_last_not_of(" \n\r\t")+1);
                    if(line.empty())
                        continue;
                    string number;
                    istringstream iss(line, istringstream::in);
                    taxelPos.zero(); taxelNorm.zero();
                    for(unsigned int j = 0; iss >> number; j++ )
                    {
                        if(j<3)
                            taxelPos[j] = strtod(number.c_str(),NULL);
                        else
                            taxelNorm[j-3] = strtod(number.c_str(),NULL);
                    }
                    skinPartWithTaxels.size++; //this is incremented for all lines - size of "port"
                    if((i>=0) && (i<=191)) //first patch - full one, 16 triangles, lower part of forearm (though in Marco's files, it is called upper)
                    {
                        if( ((i % 12) != 6) && ((i % 12) != 10)){ //every 7th and 11th taxel of a triangle are thermal pads  
                            skinPartWithTaxels.txls.push_back(new Taxel(taxelPos,taxelNorm,i));
                            printMessage(10,"Pushing taxel ID:%d, pos:%f %f %f; norm:%f %f %f.\n",i,taxelPos[0],taxelPos[1],taxelPos[2],taxelNorm[0],taxelNorm[1],taxelNorm[2]);
                        }
                    }
                    else if((i>=192) && (i<=383)){ //second patch - 7 triangles in skin V1, upper part of forearm (though in Marco's files, it is called lower)
                        if( ((i>=204) && (i<=215)) && (i!=204+6) && (i!=204+10)){ //first triangle without thermal pads
                            skinPartWithTaxels.txls.push_back(new Taxel(taxelPos,taxelNorm,i));
                            printMessage(10,"Pushing taxel ID:%d, pos:%f %f %f; norm:%f %f %f.\n",i,taxelPos[0],taxelPos[1],taxelPos[2],taxelNorm[0],taxelNorm[1],taxelNorm[2]);
                        }
                        else  if( ((i>=252) && (i<=263)) && (i!=252+6) && (i!=252+10)){ //2nd triangle without thermal pads
                            skinPartWithTaxels.txls.push_back(new Taxel(taxelPos,taxelNorm,i));
                            printMessage(10,"Pushing taxel ID:%d, pos:%f %f %f; norm:%f %f %f.\n",i,taxelPos[0],taxelPos[1],taxelPos[2],taxelNorm[0],taxelNorm[1],taxelNorm[2]);
                        }
                        else if( ((i>=288) && (i<=299)) && (i!=288+6) && (i!=288+10)){ //3rd triangle without thermal pads
                            skinPartWithTaxels.txls.push_back(new Taxel(taxelPos,taxelNorm,i));
                            printMessage(10,"Pushing taxel ID:%d, pos:%f %f %f; norm:%f %f %f.\n",i,taxelPos[0],taxelPos[1],taxelPos[2],taxelNorm[0],taxelNorm[1],taxelNorm[2]);
                        }
                        else if( ((i>=300) && (i<=311)) && (i!=300+6) && (i!=300+10)){ //4th triangle without thermal pads
                            skinPartWithTaxels.txls.push_back(new Taxel(taxelPos,taxelNorm,i));
                            printMessage(10,"Pushing taxel ID:%d, pos:%f %f %f; norm:%f %f %f.\n",i,taxelPos[0],taxelPos[1],taxelPos[2],taxelNorm[0],taxelNorm[1],taxelNorm[2]);
                        }
                        else if( ((i>=312) && (i<=323)) && (i!=312+6) && (i!=312+10)){ //5th triangle without thermal pads
                            skinPartWithTaxels.txls.push_back(new Taxel(taxelPos,taxelNorm,i));
                            printMessage(10,"Pushing taxel ID:%d, pos:%f %f %f; norm:%f %f %f.\n",i,taxelPos[0],taxelPos[1],taxelPos[2],taxelNorm[0],taxelNorm[1],taxelNorm[2]);
                        }
                        else if( ((i>=336) && (i<=347)) && (i!=336+6) && (i!=336+10)){ //6th triangle without thermal pads
                            skinPartWithTaxels.txls.push_back(new Taxel(taxelPos,taxelNorm,i));
                            printMessage(10,"Pushing taxel ID:%d, pos:%f %f %f; norm:%f %f %f.\n",i,taxelPos[0],taxelPos[1],taxelPos[2],taxelNorm[0],taxelNorm[1],taxelNorm[2]);
                        }
                        else if( ((i>=348) && (i<=359)) && (i!=348+6) && (i!=348+10)){ //7th triangle without thermal pads
                            skinPartWithTaxels.txls.push_back(new Taxel(taxelPos,taxelNorm,i));
                            printMessage(10,"Pushing taxel ID:%d, pos:%f %f %f; norm:%f %f %f.\n",i,taxelPos[0],taxelPos[1],taxelPos[2],taxelNorm[0],taxelNorm[1],taxelNorm[2]);
                        }
                    }
                }
                if(skinPartWithTaxels.size != 384){
                    yWarning("[virtContactGenerationThread]::initSkinParts():initalizing %s from file, 384 positions expected, but %d present.\n",SkinPart_s[skin_part_name].c_str(),skinPartWithTaxels.size);
                }
                skinPartWithTaxels.name = skin_part_name;
                if (skin_part_name == SKIN_LEFT_FOREARM){
                   activeSkinParts[SKIN_LEFT_FOREARM] = skinPartWithTaxels;
                   printMessage(4,"Adding SKIN_LEFT_FOREARM to activeSkinParts, it now has %d members.\n",activeSkinParts.size());
                }
                else{  // skin_part_name == SKIN_RIGHT_FOREARM
                    activeSkinParts[SKIN_RIGHT_FOREARM] = skinPartWithTaxels;
                    printMessage(4,"Adding SKIN_RIGHT_FOREARM to activeSkinParts, it now has %d members.\n",activeSkinParts.size());
                }
                break;
                
            default: 
                yError("[virtContactGenerationThread] Asked to initialize skinDynLib::SkinPart:: %d, but that skin part is not implemented yet.\n",skin_part_name);
                return -1;
        }
        
        posFile.close();
        
       
    }
    
    return 0;
    
}

void virtContactGenerationThread::printInitializedSkinParts()
{
    
    for (std::map<SkinPart,skinPartTaxel>::const_iterator it = activeSkinParts.begin() ; it != activeSkinParts.end(); ++it){
        skinPartTaxel locSkinPartTaxel = it->second;
        vector<Taxel*> taxels = locSkinPartTaxel.txls;
        printMessage(6,"Iterating through activeSkinParts (%d members), now: it->first: %d, locSkinPartTaxel.name:%d, %s.\n",activeSkinParts.size(),it->first,locSkinPartTaxel.name,SkinPart_s[locSkinPartTaxel.name].c_str());
        ofstream outFile;   
        outFile.open(SkinPart_s[locSkinPartTaxel.name].c_str());
        if (outFile.fail())          // Check for file creation and return error.
        {
           printMessage(6,"Error opening %s for output.\n",SkinPart_s[it->second.name].c_str());
           continue;
        }
        for (vector<Taxel*>::const_iterator it_taxel = taxels.begin(); it_taxel!= taxels.end(); ++it_taxel){
             outFile << (**it_taxel).Pos[0] << " " <<(**it_taxel).Pos[1] << " " <<(**it_taxel).Pos[2] << " " <<(**it_taxel).Norm[0] << " " <<(**it_taxel).Norm[1] << " " <<(**it_taxel).Norm[2] << " " <<(**it_taxel).ID <<endl; 
        }
        printMessage(6,"Wrote to file %s for output.\n",SkinPart_s[locSkinPartTaxel.name].c_str());
        outFile.close();             
    }
       
}


virtContactGenerationThread::virtContactGenerationThread(int _rate, const string &_name, const string &_robot, int _v, const string &_type, const vector<SkinPart> &_activeSkinPartsNames, const map<SkinPart,string> &_skinPartPosFilePaths): 
RateThread(_rate),name(_name), robot(_robot), verbosity(_v), type(_type), activeSkinPartsNames(_activeSkinPartsNames), skinPartPosFilePaths(_skinPartPosFilePaths)
{
   
    skinEventsOutPort = new BufferedPort<iCub::skinDynLib::skinContactList>;
  
}

bool virtContactGenerationThread::threadInit()
{
       
    ts.update();
    
    skinEventsOutPort->open(("/"+name+"/virtualContacts:o").c_str());
    
     /* initialize random seed: */
    srand (time(NULL));
    
    int returnValue = initSkinParts();
    if(returnValue == -1){
        yError("[virtContactGenerationThread] Could not initialize skin parts.\n");
        return false;  
    }
    if(verbosity > 5){
        printInitializedSkinParts();
    }
    if (activeSkinPartsNames.size() != activeSkinParts.size()){
        yError("[virtContactGenerationThread] activeSkinPartsNames and activeSkinParts have different size (%d vs. %d).\n",activeSkinPartsNames.size(),activeSkinParts.size());
        return false;    
    }
        
    return true;
}

void virtContactGenerationThread::run()
{
    ts.update();
    taxelIDinList.clear();
     
    if (type == "random"){
        skinPartIndexInVector = rand() % activeSkinPartsNames.size(); //so e.g. for size 3, this should give 0, 1, or 2, which is right
        skinPartPickedName = activeSkinPartsNames[skinPartIndexInVector];
        skinPartPicked = activeSkinParts[skinPartPickedName];
        taxelPickedIndex = rand() % skinPartPicked.txls.size(); 
        taxelPicked = *(skinPartPicked.txls[taxelPickedIndex]);
        taxelIDinList.push_back(taxelPicked.ID); //there will be only a single taxel in the list, but we want to keep the information which taxel it was
        printMessage(3,"Randomly selecting taxel ID: %d, from %s. Pose in local FoR (pos,norm): %f %f %f; norm:%f %f %f.\n",taxelPicked.ID,SkinPart_s[skinPartPickedName].c_str(),taxelPicked.Pos[0],taxelPicked.Pos[1],taxelPicked.Pos[2],taxelPicked.Norm[0],taxelPicked.Norm[1],taxelPicked.Norm[2]);  
            
        skinContact c(getBodyPart(skinPartPickedName), skinPartPickedName, getLinkNum(skinPartPickedName), taxelPicked.Pos, taxelPicked.Pos,taxelIDinList,VIRT_CONTACT_PRESSURE,taxelPicked.Norm);  
        //   skinContact(const BodyPart &_bodyPart, const SkinPart &_skinPart, unsigned int _linkNumber, const yarp::sig::Vector &_CoP, 
        //  const yarp::sig::Vector &_geoCenter, std::vector<unsigned int> _taxelList, double _pressure, const yarp::sig::Vector &_normalDir);
        printMessage(3,"Creating skin contact as follows: %s.\n",c.toString().c_str());
        
       //see also void SimulatorModule::sendSkinEvents(iCub::skinDynLib::skinContactList& skinContactListReport)
       //and compensationThread.cpp void CompensationThread::sendSkinEvents() 
       skinContactList &listWithPickedSkinContact = skinEventsOutPort->prepare();
       listWithPickedSkinContact.clear();;
       listWithPickedSkinContact.push_back(c);
       skinEventsOutPort->setEnvelope(ts);
       skinEventsOutPort->write();
    }
}

int virtContactGenerationThread::printMessage(const int l, const char *f, ...) const
{
    if (verbosity>=l)
    {
        fprintf(stdout,"[%s] ",name.c_str());

        va_list ap;
        va_start(ap,f);
        int ret=vfprintf(stdout,f,ap);
        va_end(ap);
        return ret;
    }
    else
        return -1;
}

void virtContactGenerationThread::threadRelease()
{
    printMessage(0,"Closing ports..\n");
    closePort(skinEventsOutPort);
    printMessage(1,"skinEventsOutPort successfully closed!\n");
}


