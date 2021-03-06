#ifndef WORLD_H
#define WORLD_H

#include "../common.h"
#include "model.h"
#include "ray.h"
#include "light.h"

struct HitData {
	float t;
	QVector3D normal;
	Model* model;
};

class World {
public:
    std::vector<std::shared_ptr<Model>> scene;
    std::vector<std::shared_ptr<Light>> lights;

    void add(std::shared_ptr<Model> m) {
        scene.push_back(m);
    }

    void add(std::shared_ptr<Light> l) {
        lights.push_back(l);
    }

    Model* getID(int id) {
        for (int mdx = 0; mdx < scene.size(); mdx++) {
            if (scene.at(mdx).get()->id == id) {
                return scene.at(mdx).get();
            }
        }
    }

    void removeID(int id) {
        for (int mdx = 0; mdx < scene.size(); mdx++) {
            if (scene.at(mdx).get()->id == id) {
                scene.erase(scene.begin() + mdx);
            }
        }
    }

    Model* get(int idx) {
        return scene.at(idx).get();
    }

    Light* getLight(int idx) {
        return lights.at(idx).get();
    }

    Model* get(std::string n) {
        for (int mdx = 0; mdx < scene.size(); mdx++) {
            if (scene.at(mdx).get()->name == n) {
                return scene.at(mdx).get();
            }
        }
    }

    HitData hit(Ray r) {
        bool isHit = false;
        HitData out = { INFINITY, QVector3D(0.0f, 0.0f, 0.0f), nullptr };

        for (int mdx = 0; mdx < scene.size(); mdx++) {
            Mesh mesh = scene.at(mdx).get()->mesh;

            std::vector<Vertex> data = mesh.data;

            for (int vdx = 0; vdx < data.size(); vdx += 3) {
                Vertex a = data[vdx];
                Vertex b = data[vdx + 1];
                Vertex c = data[vdx + 2];

                QVector3D A = QVector3D(QVector4D(a.position, 1.0f) * scene.at(mdx)->getModelMatrix());
                QVector3D B = QVector3D(QVector4D(b.position, 1.0f) * scene.at(mdx)->getModelMatrix());
                QVector3D C = QVector3D(QVector4D(c.position, 1.0f) * scene.at(mdx)->getModelMatrix());

                QVector3D AB = B - A;
                QVector3D BC = B - C;
                QVector3D AC = C - A;
                QVector3D P = QVector3D().crossProduct(r.direction(), AC);
                double det = QVector3D().dotProduct(AB, P);

                if (abs(det) < 0.0000001) continue;

                double invDet = 1 / det;

                QVector3D tuv = r.origin() - A;
                double u = QVector3D().dotProduct(tuv, P) * invDet;
                if (u < 0 || u > 1) continue;

                QVector3D q = QVector3D().crossProduct(tuv, AB);
                double v = QVector3D().dotProduct(r.direction(), q) * invDet;
                if (v < 0 || u + v > 1) continue;

                float t = QVector3D().dotProduct(AC, q) * invDet;

                if (t > 0.0001f && t < out.t) {
                    QVector3D normal = QVector3D().crossProduct(BC, AC).normalized();

                    if (QVector3D().dotProduct(normal, r.direction()) > 0) {
                        normal = (normal * -1.0f).normalized();
                    }

                    Model* m = scene.at(mdx).get();

                    HitData out = { t, normal, m };
                    isHit = true;
                }
            };
        }

        if (isHit) {
            return out;
        }

        return { -1, QVector3D(0.0f, 0.0f, 0.0f), nullptr };
    };
};

#endif // !WORLD_H