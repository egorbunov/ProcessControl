#include "../headers/mode.h"

// ---------------- ProcessDesc -------------------

ProcessDesc::ProcessDesc(string path) {
    this->path = path;

    path.erase(0, 1);
    path.erase(path.length() - 1, 1);

    size_t index = path.find_last_of('/');
    if (index != string::npos && index + 1 < path.length())
        this->name = path.substr(index + 1);
    else
        this->name = path;
}

string ProcessDesc::getName() const {
    return name;
}

string ProcessDesc::getPath() const {
    return path;
}


// --------- Mode ------------

string Mode::getName() const {
    return name;
}

string Mode::getDescription() const {
    return description;
}

const vector<string>& Mode::getUrls() const{
    return urls;
}

const vector<ProcessDesc>& Mode::getProcesses() const {
    return processes;
}

short Mode::getProgress() const {
    return progress;
}

void Mode::setName(string name) {
    this->name = name;
}

void Mode::setDescription(string description) {
    this->description = description;
}

void Mode::setProgress(short progress) {
    this->progress = progress;
}

void Mode::addUrl(string url) {
    this->urls.push_back(url);
}

void Mode::addProcess(ProcessDesc pd) {
    this->processes.push_back(pd);
}

void Mode::addProcess(string path) {
    this->processes.push_back(ProcessDesc(path));
}

size_t Mode::getUrlCount() const {
    return urls.size();
}

size_t Mode::getProcessCount() const {
    return processes.size();
}

Mode::Mode() {
}

Mode::Mode(stringstream &stream) {
    stringstream ss;
    processes.clear();
    urls.clear();

    //reading name and description
    std::getline(stream, this->name);
    std::getline(stream, this->description);

    string str, proc, url;
    // reading processes
    std::getline(stream, str);
    ss.str(str);
    while (!ss.eof())
    {
        std::getline(ss, proc, Mode::DELIMITER);
        if (proc != "")
            this->addProcess(proc);
    }
    ss.clear();

    // reading urls
    std::getline(stream, str);
    ss.str(str);
    while (!ss.eof())
    {
        std::getline(ss, url, Mode::DELIMITER);
        if (url != "")
            this->addUrl(url);
    }
    ss.clear();
    // reading progress
    stream >> this->progress;
    stream.get(); // endl sym
}

std::ostream& operator <<(std::ostream& stream, const Mode& mode) {
    stream << mode.getName() << std::endl;
    stream << mode.getDescription() << std::endl;
    const vector<ProcessDesc> &processes = mode.getProcesses();
    for (int i = 0; i < processes.size(); ++i)
        stream << processes[i].getPath() << Mode::DELIMITER;
    stream << std::endl;
    const vector<string> &urls = mode.getUrls();
    for (int i = 0; i < urls.size(); ++i)
    {
        stream << urls[i] << Mode::DELIMITER;
    }
    stream << std::endl;
    stream << mode.getProgress() << std::endl;

    return stream;
}