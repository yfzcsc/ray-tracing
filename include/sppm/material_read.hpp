#ifndef _MATERIAL_READ_H___
#define _MATERIAL_READ_H___

#include "material.hpp"
#include "image.hpp"
#include <string>
#include <map>
#include <memory>

class MaterialReader {
    public:
        MaterialReader() = delete;
        MaterialReader(double specular, double diffuse, double refraction, double refractionIndex, std::string path,  std::string filename) { load(path, filename, specular, diffuse, refraction, refractionIndex); }
        void load(std::string path, std::string filename, double specular, double diffuse, double refraction, double refractionIndex);
        std::shared_ptr<Material> getMaterial(std::string mat_name) { return stM[mat_name]; }
        std::shared_ptr<Image> getColFile(std::string col_name) { return stC.count(col_name) ? stC[col_name] : nullptr; }

    private:
        std::map<std::string, std::shared_ptr<Material>> stM;
        std::map<std::string, std::shared_ptr<Image>> stC;
};

#endif