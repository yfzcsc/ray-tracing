#include "material_read.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include "image.hpp"

void MaterialReader::load(std::string path, std::string filename, double specular, double diffuse, double refraction, double refractionIndex) {
    std::ifstream in(filename);
    std::string line;
    if (!in.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    while (!in.eof()) {
        std::getline(in, line);
        // puts(line.c_str());
        std::stringstream ss(line);
        std::string token;
        std::string name;
        ss >> token;
        double Ns = 0, Ni = 1, d = 1;
        Vector3f Ka = Vector3f::ZERO, Kd = Vector3f(0.8, 0.8, 0.8), Ks = Vector3f::ZERO;
        int illum = 2;

        if(token == "newmtl") {
            ss >> name;
            while(1){
                std::getline(in, line);
                std::stringstream ss2(line);
                std::string rdname;
                if(ss2 >> rdname) {
                    if(rdname == "Ka") {
                        ss2 >> Ka[0] >> Ka[1] >> Ka[2];
                    } else if(rdname == "Kd") {
                        ss2 >> Kd[0] >> Kd[1] >> Kd[2];
                    } else if(rdname == "Ks") {
                        ss2 >> Ks[0] >> Ks[1] >> Ks[2];
                    } else if(rdname == "Ns") {
                        ss2 >> Ns;
                    } else if(rdname == "Ni") {
                        ss2 >> Ni;
                    } else if(rdname == "d") {
                        ss2 >> d;
                    } else if(rdname == "illum") {
                        ss2 >> illum;
                    } else if(rdname == "map_Kd") {
                        std::string mapfile;
                        ss2 >> mapfile;
                        std::cerr << "Read: " << (path + mapfile) << std::endl;
                        stC[name] = mapfile.substr(mapfile.length() - 4, 4) == ".tga" ? std::shared_ptr<Image>(Image::LoadTGA((path + mapfile).c_str())) : std::shared_ptr<Image>(Image::LoadPPM((path + mapfile).c_str()));
                    }
                } else break; 
            }
            stM[name] = std::make_shared<Material>(specular, diffuse, refraction, Ka, Kd, Ks, Ns, refractionIndex);
        }
    }
    in.close();
}