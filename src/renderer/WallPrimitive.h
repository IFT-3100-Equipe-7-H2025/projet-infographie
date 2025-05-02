#include <of3dPrimitives.h>
#include <ofImage.h>
#include <ofNode.h>
#include <ofTexture.h>
class WallPrimitive : public of3dPrimitive
{
public:
    ofTexture texture;

    explicit WallPrimitive(const std::shared_ptr<ofMesh>& mesh)//, ofTexture tex)
    {
        this->mesh = mesh;
        //this->texture = tex;
        enableTextures();
        enableNormals();
    }

    void draw() const override
    {
        //texture.bind();
        of3dPrimitive::draw();
        //texture.unbind();
    }
};
