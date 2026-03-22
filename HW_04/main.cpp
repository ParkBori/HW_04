#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <functional>


/*
* 2026.3.22
* [작성자] : 박재현 
* 4번 과제
* 
* 물약 추가시 자동으로 재고가 채워지는 기능을 
* std::function을 사용해 이벤트 기반으로 처리
*/



namespace MyUtil
{
    template<typename T = int>
    [[nodiscard]] inline T Clamp(T Value, T Min, T Max)
    {
        if (Value < Min)
        {
            return Min;
        }
        else if (Max < Value)
        {
            return Max;
        }

        return Value;
    }
}

// PotionRecipe 클래스: 재료 목록을 vector<string>으로 변경
class PotionRecipe
{
public:
    // 생성자: 재료 목록을 받아 초기화하도록 수정
    PotionRecipe(const std::string& name, const std::vector<std::string>& ingredients)
        : potionName(name), ingredients(ingredients)
    {
    }

public:
    std::string potionName;
    std::vector<std::string> ingredients; // 단일 재료에서 재료 '목록'으로 변경

};

class RecipeManager
{
public:
    PotionRecipe* AddRecipe(const std::string& name, std::vector<std::string> ingredients)
    {
        PotionRecipe NewRecipe(name, ingredients);
        recipes.push_back(NewRecipe);
        
        // 포션 추가 이벤트
        onPotionRecipeAdded(name);
        std::cout << ">> 새로운 레시피 '" << name << "'이(가) 추가되었습니다." << std::endl;

        return &NewRecipe;
    }

    PotionRecipe* FindRecipeByName(const std::string& name)
    {
        std::vector<PotionRecipe> ::iterator it = std::find_if(recipes.begin(), recipes.end(), [name](const PotionRecipe& Potion) {
            return Potion.potionName == name;
            });

        return &(*it);
    }

    std::vector<PotionRecipe> FindRecipeByIngredient(const std::string& ingredient)
    {
        std::vector<PotionRecipe> exactPotionRecipes;

        for (const auto& recipe : recipes)
        {
            for (const auto& i : recipe.ingredients)
            {
                if (i == ingredient)
                {
                    exactPotionRecipes.push_back(recipe);
                    break;
                }
            }
        }

        return exactPotionRecipes;
    }

    const std::vector<PotionRecipe>& GetAllRecipes() const
    {
        return recipes;
    }

private:
    std::vector<PotionRecipe> recipes;

public:
    // 물약 추가 이벤트 
    std::function<void(const std::string&)> onPotionRecipeAdded;
};

class StockManager
{
public:

    bool DispensePotion(const std::string& potionName)
    {
        if (potionStock.end() == potionStock.find(potionName))
        {
            return false;
        }

        if (potionStock[potionName] <= 0)
        {
            return false;
        }

        potionStock[potionName] = MyUtil::Clamp<int>(potionStock[potionName] - 1, 0, MAX_STOCK);
        return true;
    }

    void ReturnPotion(const std::string& potionName)
    {
        if (potionStock.end() == potionStock.find(potionName))
        {
            return;
        }

        potionStock[potionName]  = MyUtil::Clamp<int>(potionStock[potionName] + 1, 0, MAX_STOCK);
    }
    
    int GetStock(const std::string& potionName)
    {
        if (potionStock.end() == potionStock.find(potionName))
        {
            return -1;
        }

        return potionStock[potionName];
    }

    void addedNewRecipe(const std::string newRecipe)
    {
        potionStock.insert({ newRecipe, MAX_STOCK });
    }

private:
    std::map<std::string, int> potionStock;
    const int MAX_STOCK = 3;
};


// AlchemyWorkshop 클래스: 레시피 목록을 관리
class AlchemyWorkshop 
{
public:
    AlchemyWorkshop()
    {
        myRecipManager.onPotionRecipeAdded = [this](const std::string& newRecipeName) -> void {
            myStockManager.addedNewRecipe(newRecipeName);
        };
    }
 
public:
    void AddRecipe(const std::string& name, const std::vector<std::string>& ingredients)
    {
        myRecipManager.AddRecipe(name, ingredients);
    }

    void DisplayAllRecipes() const
    {
        const auto& recipes =  myRecipManager.GetAllRecipes();
        if (recipes.empty()) 
        {
            std::cout << "아직 등록된 레시피가 없습니다." << std::endl;
            return;
        }

        std::cout << "\n--- [ 전체 레시피 목록 ] ---" << std::endl;
        for (const auto& recipe : recipes)
        {
            std::cout << "- 물약 이름: " << recipe.potionName << std::endl;
            std::cout << "  > 필요 재료: ";

            // 재료 목록을 순회하며 출력
            for (size_t j = 0; j < recipe.ingredients.size(); ++j)
            {
                std::cout << recipes[j].ingredients[j];
                // 마지막 재료가 아니면 쉼표로 구분
                if (j < recipes[j].ingredients.size() - 1)
                {
                    std::cout << ", ";
                }
            }
            std::cout << std::endl;

        }
        std::cout << "---------------------------\n";
    }

    int GetStockByName(const std::string& Name)
    {
        return myStockManager.GetStock(Name);

    }

    bool DispensePotionByName(const std::string& potionName)
    {
        return myStockManager.DispensePotion(potionName);
    }

    std::vector<std::string> DispensePotionsByIngredient(const std::string& ingredient)
    {
        std::vector<std::string> DispensePotionNames;

        // 1. 포션 재료로 레시피 찾기 
        const std::vector<PotionRecipe>& Recipes = myRecipManager.FindRecipeByIngredient(ingredient);

        // 2. 포션 이름 
        for (auto& Recipe : Recipes)
        {
            if (myStockManager.DispensePotion(Recipe.potionName))
            {
                DispensePotionNames.push_back(Recipe.potionName);
            }
        }
        
        return DispensePotionNames;
    }


    void ReturnPotionByName(const std::string& potionName)
    {
        myStockManager.ReturnPotion(potionName);
    }

private:
    RecipeManager myRecipManager;
    StockManager myStockManager;
};


int main()
{
    AlchemyWorkshop workshop;

    workshop.AddRecipe("Healing Potion", { "Herb", "Water" });
    workshop.AddRecipe("Mana Potion", { "Magic Water", "Crystal" });
    workshop.AddRecipe("Stamina Potion", { "Herb", "Berry" });
    workshop.AddRecipe("Fire Resistance Potion", { "Fire Flower", "Ash" });

    std::cout << "=== 초기 상태 (레시피 추가 + 재고 자동 3개) ===\n";
    workshop.DisplayAllRecipes();

    std::cout << "\n[재고 확인] Healing Potion 재고: "
        << workshop.GetStockByName("Healing Potion") << "\n";

    std::cout << "\n=== 이름으로 지급 테스트 (Healing Potion 3회 지급) ===\n";
    std::cout << "1회 지급: " << (workshop.DispensePotionByName("Healing Potion") ? "성공" : "실패") << "\n";
    std::cout << "2회 지급: " << (workshop.DispensePotionByName("Healing Potion") ? "성공" : "실패") << "\n";
    std::cout << "3회 지급: " << (workshop.DispensePotionByName("Healing Potion") ? "성공" : "실패") << "\n";

    std::cout << "현재 재고: " << workshop.GetStockByName("Healing Potion") << "\n";

    std::cout << "4회 지급(재고 없으면 실패): "
        << (workshop.DispensePotionByName("Healing Potion") ? "성공" : "실패") << "\n";

    std::cout << "\n=== 재료로 지급 테스트 (ingredient = Herb) ===\n";
    std::vector<std::string> dispensed = workshop.DispensePotionsByIngredient("Herb");

    std::cout << "지급된 물약 수: " << dispensed.size() << "\n";
    for (const std::string& name : dispensed)
        std::cout << "- " << name << "\n";

    std::cout << "\n=== 공병 반환 테스트 (Healing Potion) ===\n";
    workshop.ReturnPotionByName("Healing Potion");
    workshop.ReturnPotionByName("Healing Potion");
    workshop.ReturnPotionByName("Healing Potion"); // 이미 3이면 더 올라가면 안 됨


    std::cout << "반환 후 재고(최대 3 유지): "
        << workshop.GetStockByName("Healing Potion") << "\n";

    std::cout << "\n=== 최종 상태 ===\n";
    workshop.DisplayAllRecipes();
}
