#include <bits/stdc++.h>
#include <ctime>
#include <getopt.h>

using namespace std;

map<int, map<int, pair<int, bool>>> occupiedpages; //pid, process page, page in memory, swap/main (false for swap, true for main memory)

vector<vector<int>> mainstorage;
vector<vector<int>> swapstorage;

unordered_map<int, int> processpagesize; //No of pages of each process: pid, pagereq
set<int> activepid; //Set of all pids currently in memory

unordered_map<int, string> pidtofile; //Maps pid to filename

set<int> freemainpages; //Set of available free pages
set<int> freevirtualpages; //Set of available virtual memory pages
set<pair<int, int>> fifo; //For pages in main memory, 1.usetime, 2.main memory page
map<pair<int, bool>, pair<int, int>> ppagetolpage; //(ppage, swap/main) -> (pid, lpage)

unordered_map<int, int> ppagetousetime; //Gives main page in use to usetime conversion

int pid = 1; //Global pid counter

int usetime = 1;

int mainpagecount; //No of main memory pages
int virtualpagecount; //No of virtual memory pages
int mainmemory = 0; //in kb
int virtualmemory = 0; //in kb
int pagesize = 0; //in bytes
string inpfile;
string outfile;

ofstream mainoutput;

//How to handle load file1, file1???

//To initialize the main memory pages and put them in the queue
void initPages(){
    for(int i = 0; i<mainpagecount; i++){
        freemainpages.insert(i);
    }

    for(int i = 0; i<virtualpagecount; i++){
        freevirtualpages.insert(i);
    }
}

void loadfiles(vector<string> filenames){
    for(auto filename : filenames){
        ifstream file(filename);
        if (!file.is_open()) {
            mainoutput << filename << " could not be loaded - file does not exist" << endl;
            continue;
        }
        
        int filesize;
        file >> filesize;

        int pagereq = (filesize*1024)/pagesize;
        //Checking if memory can accomodate 
        if((freemainpages.size() + freevirtualpages.size()) < pagereq){
            //Process cannot be loaded
            mainoutput<<filename<<" could not be loaded - memory is full"<<endl;
        }
        else{
            pidtofile[pid] = filename; //setting pid to filename mapping
            int allocatedpages = 0;

            while (allocatedpages < pagereq && !freemainpages.empty()) {
                auto it = freemainpages.begin();
                int page = *it;
                freemainpages.erase(it);
                occupiedpages[pid][allocatedpages] = {page, true};  // true- main moemory
                ppagetolpage[{page,true}] = {pid, allocatedpages};
                fifo.insert({usetime, page});
                ppagetousetime[page] = usetime;
                usetime++;
                allocatedpages++;
            }

            while(allocatedpages < pagereq && !freevirtualpages.empty()){
                auto it = freevirtualpages.begin();
                int page = *it;
                freevirtualpages.erase(it);
                occupiedpages[pid][allocatedpages] = {page, false}; //false swap memory
                ppagetolpage[{page,false}] = {pid, allocatedpages};
                allocatedpages++;
            }
            processpagesize[pid] = pagereq;
            activepid.insert(pid);
            mainoutput<<filename<<" is loaded and is assigned process id "<<pid<<endl;
            pid++;
        }
        file.close();
    }
}

int getpage(int lpage, int pid){
    if(lpage < processpagesize[pid]){ //check if logical page number is valid
        int ppage = occupiedpages[pid][lpage].first;
        if(occupiedpages[pid][lpage].second){ //page in main memory
            return ppage;
        }
        else{ //page in swap must be brought in memory
            //What if main memory is available
            if(freemainpages.size()>0){
                auto freemainpg_it = freemainpages.begin(); //Fetching free main page
                int freemainpg = (*freemainpg_it);
                freemainpages.erase(freemainpg_it);

                swap(mainstorage[freemainpg], swapstorage[ppage]); //Swapping the page values in memory

                occupiedpages[pid][lpage] = {freemainpg, true}; //Assigning new page to lpage
                ppagetolpage[{freemainpg, true}] = {pid, lpage}; //updating ppage to lpage map
                
                fifo.insert({usetime, freemainpg}); //Adding new page to fifo queue
                ppagetousetime[freemainpg] = usetime;
                usetime++;

                ppagetolpage.erase({ppage, false}); //removing ppage to lpage mapping
                freevirtualpages.insert(ppage); //Because now this is a free virtual page

            }
            else{//let page to be swapped from main memory be pg1, and page to be brought is ppage
                auto fifo_it = fifo.begin();
                //int pg1usetime = fifo_it->first;
                int pg1 = fifo_it->second; //page to be removed according to fifo
                int pg1pid = ppagetolpage[{pg1, true}].first; //pid that holds that page
                int pg1lpage = ppagetolpage[{pg1, true}].second; //lpage corresponding to that page

                swap(mainstorage[pg1], swapstorage[ppage]); //Swapping the page values in memory

                occupiedpages[pg1pid][pg1lpage] = {ppage, false}; //Now the other process corresponds to the page in swap memory
                occupiedpages[pid][lpage] = {pg1, true}; //And current process corresponds to the page in main memory

                ppagetolpage[{ppage, false}] = {pg1pid, pg1lpage}; //Setting ppage to lpage values as well
                ppagetolpage[{pg1, true}] = {pid, lpage};

                fifo.erase(fifo_it);
                fifo.insert({usetime, pg1}); //Re-inserting the page with new usetime
                ppagetousetime[pg1] = usetime;
                usetime++;
            }
            return occupiedpages[pid][lpage].first; //Re-inserting 
        }
    }
    else{
        return -1;  //invalid logical page
    }
}

void runprocess(int pid){
    if(activepid.find(pid)!=activepid.end()){ //If process with that pid is in memory
        string filename = pidtofile[pid];
        ifstream file(filename);
        if(!file.is_open()){
            cout<<"Error: File Stream not open"<<endl;
            return;
        }
        int filesize;//Just for reading the first line
        file >> filesize;
        string line;
        while(getline(file, line)){
            stringstream ss(line);
            string command;
            ss>>command;

            if(command == "load"){
                int address, value;
                string valwithcomma, addrstring;
                ss >> valwithcomma >>addrstring;
                valwithcomma.pop_back();
                value = stoi(valwithcomma);
                address = stoi(addrstring);
                //let's assume we have these values
                int lpage = address/pagesize; //getting logical page number 
                int offset = address%pagesize; //getting offset
                int getmainpage = getpage(lpage, pid);
                if(getmainpage!=-1){
                    mainstorage[getmainpage][offset] = value;
                    //Print something
                    mainoutput<<"Command: load "<<value<<", "<<address<<"; ";
                    mainoutput<<"Result: Value of "<<mainstorage[getmainpage][offset]<<" is now stored in addr "<<address<<endl;
                }
                else{
                    mainoutput<<"Invalid Memory Address "<<address<<" specified for process id "<<pid<<endl;
                    file.close();
                    return;
                }
            }
            else if(command == "add" || command == "sub"){
                int xaddress, yaddress, zaddress;
                int xvalue, yvalue, zvalue;
                string xwithcomma, ywithcomma, zstring;
                ss >> xwithcomma >> ywithcomma >> zstring;
                xwithcomma.pop_back();
                ywithcomma.pop_back();
                xaddress = stoi(xwithcomma);
                yaddress = stoi(ywithcomma);
                zaddress = stoi(zstring);
                //let's assume we have these values

                //For x
                int lpage = xaddress/pagesize; //getting logical page number 
                int offset = xaddress%pagesize; //getting offset
                int getmainpage = getpage(lpage, pid);
                if(getmainpage!=-1){
                    xvalue = mainstorage[getmainpage][offset];
                }
                else{
                    mainoutput<<"Invalid Memory Address "<<xaddress<<" specified for process id "<<pid<<endl;
                    file.close();
                    return;
                }

                //For y
                lpage = yaddress/pagesize; //getting logical page number 
                offset = yaddress%pagesize; //getting offset
                getmainpage = getpage(lpage, pid);
                if(getmainpage!=-1){
                    yvalue = mainstorage[getmainpage][offset];
                }
                else{
                    mainoutput<<"Invalid Memory Address "<<yaddress<<" specified for process id "<<pid<<endl;
                    file.close();
                    return;
                }

                //for z
                lpage = zaddress/pagesize; //getting logical page number 
                offset = zaddress%pagesize; //getting offset
                getmainpage = getpage(lpage, pid);
                if(getmainpage!=-1){
                    if(command == "add"){
                        mainstorage[getmainpage][offset] = xvalue + yvalue;
                        zvalue = mainstorage[getmainpage][offset];
                        //print something for add
                        mainoutput<<"Command: add "<<xaddress<<", "<<yaddress<<", "<<zaddress<<"; ";
                        mainoutput<<"Result: Value in addr "<<xaddress<<" = "<<xvalue<<", addr "<<yaddress<<" = "<<yvalue<<", addr "<<zaddress<<" = "<<zvalue<<endl;
                    }
                    else if(command == "sub"){
                        mainstorage[getmainpage][offset] = xvalue - yvalue;
                        zvalue = mainstorage[getmainpage][offset];
                        //print something for sub
                        mainoutput<<"Command: sub "<<xaddress<<", "<<yaddress<<", "<<zaddress<<"; ";
                        mainoutput<<"Result: Value in addr "<<xaddress<<" = "<<xvalue<<", addr "<<yaddress<<" = "<<yvalue<<", addr "<<zaddress<<" = "<<zvalue<<endl;
                    }
                }
                else{
                    mainoutput<<"Invalid Memory Address "<<zaddress<<" specified for process id "<<pid<<endl;
                    file.close();
                    return;
                }
            }
            else if(command == "print"){
                int address;
                string addrstring;
                ss >> addrstring;
                address = stoi(addrstring);
                //let's assume we have these values

                int lpage = address/pagesize; //getting logical page number 
                int offset = address%pagesize; //getting offset
                int getmainpage = getpage(lpage, pid);
                if(getmainpage!=-1){
                    mainoutput<<"Command: print "<<address<<"; ";
                    mainoutput<<"Result: Value in addr "<<address<<" = "<<mainstorage[getmainpage][offset]<<endl;
                }
                else{
                    mainoutput<<"Invalid Memory Address "<<address<<" specified for process id "<<pid<<endl;
                    file.close();
                    return;
                }
            }
        }
    }
    else{ //No process with that pid in memory
        mainoutput<<"Invalid PID "<<pid<<endl;
    }
}

void killprocess(int pid){
    //If pid exists
    if(activepid.find(pid)!=activepid.end()){
        for(auto& process : occupiedpages[pid]){
            //int lpage = process.first;
            int ppage = process.second.first;
            if(process.second.second){//page is in main memory
                int usetime = ppagetousetime[ppage];//removing page from fifo list
                fifo.erase(fifo.find({usetime, ppage}));
                freemainpages.insert(ppage);
                ppagetolpage.erase({ppage, true});
                ppagetousetime.erase(ppage);    
            }
            else{
                freevirtualpages.insert(ppage);
                ppagetolpage.erase({ppage, false});
            }
        }
        string filename = pidtofile[pid];
        pidtofile.erase(pid);
        activepid.erase(activepid.find(pid));
        processpagesize.erase(pid);
        occupiedpages.erase(pid);
        mainoutput<<"killed "<<pid<<endl;
    }
    else{
        mainoutput<<"Invalid PID "<<pid<<endl;
    }
    
}

void listprocess(){
    for(int it : activepid){
        mainoutput<<it<<" ";
    }
    mainoutput<<endl;
}

void printpte(int pid, string filename){
    ofstream outputfile(filename, ios::app);
    if(!outputfile.is_open()){
        cout<<"Failed to open "<<filename<<endl;
        return;
    }
    if (activepid.find(pid) == activepid.end()) {
        mainoutput<<"Invalid PID "<<pid<<endl;
        outputfile.close(); // It's good practice to close the file
        return;
    }

    //PRINT DATE AND TIME FIRST
    time_t currentTime = time(nullptr);
    // Convert time to struct tm (local time)
    tm* localTime = localtime(&currentTime);
    // Create a buffer to hold the formatted date and time
    char buffer[100];
    // Format the date and time using strftime
    // This format corresponds to the Linux `date` command default output (e.g., Tue Oct 19 15:46:33 2024)
    strftime(buffer, sizeof(buffer), "%a %b %d %T %Y", localTime);
    // Print the formatted date and time
    outputfile << "Current date and time: " << buffer << endl;

    for(auto process : occupiedpages[pid]){
        int lpage = process.first;
        int ppage = process.second.first;
        bool swapmain = process.second.second;
        outputfile<<lpage<<" "<<ppage<<" "<<swapmain<<endl;
    }
    outputfile.close();
}

void printpteall(string filename){
    ofstream outputfile(filename, ios::app);
    if(!outputfile.is_open()){
        cout<<"Failed to open "<<filename<<endl;
        return;
    }

    //PRINT DATE AND TIME FIRST
    time_t currentTime = time(nullptr);
    // Convert time to struct tm (local time)
    tm* localTime = localtime(&currentTime);
    // Create a buffer to hold the formatted date and time
    char buffer[100];
    // Format the date and time using strftime
    // This format corresponds to the Linux `date` command default output (e.g., Tue Oct 19 15:46:33 2024)
    strftime(buffer, sizeof(buffer), "%a %b %d %T %Y", localTime);
    // Print the formatted date and time
    outputfile << "Current date and time: " << buffer << endl;

    for(auto entry : occupiedpages){
        int pid = entry.first;
        for(auto process : entry.second){
            int lpage = process.first;
            int ppage = process.second.first;
            bool swapmain = process.second.second;
            outputfile<<pid<<" "<<lpage<<" "<<ppage<<" "<<swapmain<<endl;
        }
    }
    outputfile.close();
}

void print(int memloc, int length){
    int valuesprinted = 0;
    while(valuesprinted<length){
        int address = memloc + valuesprinted;
        int ppage = address/pagesize;
        int offset = address%pagesize;
        if(ppage<mainpagecount){
            mainoutput<<"Value of "<<address<<": "<<mainstorage[ppage][offset]<<endl;
        }
        else{
            //Invalid address
            mainoutput<<"Invalid Memory Address "<<address<<endl;
            return;
        }
    }
}

int main(int argc, char *argv[]){
    int opt;
    
    static struct option long_options[] = {
        {"mainmemory", required_argument, 0, 'M'},
        {"virtualmemory", required_argument, 0, 'V'},
        {"pagesize", required_argument, 0, 'P'},
        {"inpfile", required_argument, 0, 'i'},
        {"outfile", required_argument, 0, 'o'},
        {0, 0, 0, 0}
    };

    while((opt = getopt_long(argc, argv, "M:V:P:i:o:", long_options, nullptr))!=-1){
        switch(opt){
            case 'M':
                mainmemory = stoi(optarg);
                break;
            
            case 'V':
                virtualmemory = stoi(optarg);
                break;

            case 'P':
                pagesize = stoi(optarg);
                break;

            case 'i':
                inpfile = optarg;
                break;

            case 'o':
                outfile = optarg;
                break;

            default:
                mainoutput<<"Incorrect cmdline args"<<endl;
                break;
        }
    }

    mainpagecount = (mainmemory*1024)/pagesize;
    virtualpagecount = (virtualmemory*1024)/pagesize;
    cout<<"Main Memory Pages: "<<mainpagecount<<" Virtual Memory Pages: "<<virtualpagecount<<endl;
    mainstorage.resize(mainpagecount, vector<int>(pagesize, 0)); //Setting up actual main memory
    swapstorage.resize(virtualpagecount, vector<int>(pagesize, 0)); //Setting up actual swap space

    initPages(); //Adding pages to the free queue

    ifstream inputfile(inpfile);
    mainoutput.open(outfile);
    if (!inputfile.is_open()) {
        //Ask about proper error message, for ifstream not working
        cout << inpfile << " input file could not be opened" << endl;
        return 0;
    }
    if (!mainoutput.is_open()) {
        //Ask about proper error message, for ifstream not working
        cout << outfile << " output file could not be opened" << endl;
        return 0;
    }
    string line;
    while(getline(inputfile, line)){
        stringstream ss(line);
        string command;
        ss >> command;
        if(command == "load"){
            vector<string> filenames;
            string filename;
            while(ss >> filename){
                filenames.push_back(filename);
            }
            loadfiles(filenames);
        }
        else if(command == "run"){
            int pid;
            ss >> pid;
            runprocess(pid);
        }
        else if(command == "kill"){
            int pid;
            ss >> pid;
            killprocess(pid);
        }
        else if(command == "listpr"){
            listprocess();
        }
        else if(command == "pte"){
            int pid;
            string filename;
            ss >> pid >> filename;
            printpte(pid, filename);
        }
        else if(command == "pteall"){
            string filename;
            ss >> filename;
            printpteall(filename);
        }
        else if(command == "print"){
            int memloc, length;
            ss>>memloc>>length;
            print(memloc, length);
        }
        else if(command == "exit"){
            //exit
            inputfile.close();
            exit(1);
        }
        else{
            cout<<"Unknown command: "<<command<<endl;
        }
    }
    inputfile.close();
    mainoutput.close();
    return 0;
}