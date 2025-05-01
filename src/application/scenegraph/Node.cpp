#include "Node.h"

#include "Light.h"
#include "material/ShaderMaterial.h"
#include "of3dPrimitives.h"
#include <memory>
#include <vector>

Node::Node(std::string name, std::shared_ptr<ofNode> node) : inner(std::move(node)), name(std::move(name)), id(nextId++)
{}

bool Node::AddChild(std::shared_ptr<Node> child)
{
    if (child->GetId() == this->id)
    {
        ofLogError() << "Cannot add a node as a child of itself";
        return false;
    }

    child->inner->setParent(*this->inner);
    child->parent = shared_from_this();
    children.push_back(std::move(child));
    return true;
}

void Node::RemoveChild(NodeId id)
{
    for (auto it = children.begin(); it != children.end(); ++it)
    {
        if ((*it)->GetId() == id)
        {
            (*it)->inner->clearParent();
            (*it)->parent.reset();
            children.erase(it);
            return;
        }
    }
}

bool Node::Delete()
{
    if (this->GetId() == 0)
    {
        ofLogError() << "Cannot delete the root node";
        return false;
    }

    for (const auto& child: this->children)
    {
        child->parent.reset();
    }

    this->children.clear();

    if (!this->parent.expired())
    {
        this->parent.lock()->RemoveChild(this->id);
        return true;
    }

    return false;
}

// NOLINTNEXTLINE(misc-no-recursion)
void Node::Draw(const std::vector<std::shared_ptr<Light>>& lights, const std::shared_ptr<Shader>& lightingModel) const
{
    if (inner)
    {
        if (material && lightingModel == nullptr && !std::dynamic_pointer_cast<PBRMaterial>(this->material))
        {
            material->begin();
        }
        else if (auto material_ptr = std::dynamic_pointer_cast<DefaultMaterial>(this->material); lightingModel && material_ptr)
        {
            lightingModel->begin();

            glm::vec3 lightPosition = lights.empty() ? glm::vec3(0.0f) : lights[0]->getGlobalPosition();
            glm::mat4 viewMatrix = ofGetCurrentMatrix(OF_MATRIX_MODELVIEW);
            glm::vec4 lightPosInViewSpace = viewMatrix * glm::vec4(lightPosition, 1.0);

            // For Gooch
            if (lightingModel->GetName() == "Gooch")
            {
                glm::mat4 model = inner->getGlobalTransformMatrix();
                glm::mat3 normalMat = glm::inverseTranspose(glm::mat3(model));
                lightingModel->setUniformMatrix4f("modelMatrix", model);
                lightingModel->setUniformMatrix3f("normalMatrix", normalMat);
                lightingModel->setUniform3f("uLightPos", lightPosition);
                lightingModel->setUniform3f("uSurfaceColor", 1.0, 0.5, 1.0);
                lightingModel->setUniform3f("uWarmColor", 1.0, 0.6, 0.0);
                lightingModel->setUniform3f("uCoolColor", 0.0, 0.0, 1.0);
                lightingModel->setUniform1f("uAlpha", 0.25f);
                lightingModel->setUniform1f("uBeta", 0.25f);
            }
            // For Toon
            else if (lightingModel->GetName() == "Toon")
            {
                lightingModel->setUniform3f("lightPosEye", lightPosInViewSpace);
                lightingModel->setUniform3f("baseColor", glm::vec3(0.55f, 0.27f, 0.07f));// cocoa
                lightingModel->setUniform1i("bandCount", 3);                             // 2-5 works nicely
                lightingModel->setUniform1f("rimWidth", 0.25f);
                lightingModel->setUniform1f("rimPower", 1.8f);
            }
            // For Lambert, Gouraud, Phong and Blinn-Phong
            else
            {
                auto ambientColor = material_ptr->GetMaterial()->getAmbientColor();
                auto diffuseColor = material_ptr->GetMaterial()->getDiffuseColor();
                auto specularColor = material_ptr->GetMaterial()->getSpecularColor();
                auto shininess = material_ptr->GetMaterial()->getShininess();

                lightingModel->setUniform3f("color_ambient", ambientColor.r, ambientColor.g, ambientColor.b);
                lightingModel->setUniform3f("color_diffuse", diffuseColor.r, diffuseColor.g, diffuseColor.b);
                lightingModel->setUniform3f("color_specular", specularColor.r, specularColor.g, specularColor.b);
                lightingModel->setUniform1f("brightness", shininess);
                lightingModel->setUniform3f("light_position", glm::vec3(lightPosInViewSpace));
            }
        }
        else if (auto material_ptr = std::dynamic_pointer_cast<PBRMaterial>(this->material); material_ptr)
        {
            material_ptr->begin();
            material_ptr->SetUniforms(lights);
        }
        inner->draw();
        if (lightingModel) { lightingModel->end(); }
        if (material && (lightingModel == nullptr || std::dynamic_pointer_cast<PBRMaterial>(this->material)))
        {
            material->end();
        }
    }
    for (const auto& child: children)
    {
        child->Draw(lights, lightingModel);
    }
}

void Node::SetMaterial(std::shared_ptr<Material> material)
{
    if (material == nullptr)
    {
        ofLogError() << "Cannot set material to nullptr";
        return;
    }
    this->material = std::move(material);
}

[[nodiscard]] const std::shared_ptr<Material>& Node::GetMaterial() const
{
    return this->material;
}

[[nodiscard]] std::string Node::GetName() const
{
    return name + " " + std::to_string(id);
}
[[nodiscard]] const std::vector<std::shared_ptr<Node>>& Node::GetChildren() const { return children; }
[[nodiscard]] NodeId Node::GetId() const { return id; }
[[nodiscard]] const std::weak_ptr<Node>& Node::GetParent() const { return this->parent; }

void Node::SetOpen(bool open) { isOpen = open; }

[[nodiscard]] bool Node::IsOpen() const { return isOpen; }

[[nodiscard]] const std::shared_ptr<ofNode>& Node::GetInner() const { return inner; }
