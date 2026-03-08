#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <format>

/*
* 2026.3.28
* [작성자] : 박재현 
* 
* 1. 기존 코드 분석하기 
*   어던 의도로 이 코드를 구현했을까?
*   레시피 - 포션 레시피 
*       다양한 물약에 대응하기 위해 레시피 재료들을 문자열로 받고있음
*       
*       
*
*   연금술 상점 - 포션 레시피 보여주기 and 포션 레시피 만들기
*       사용자가 레시피를 직접 만들수 있고 그 결과를 즉시 볼수 있음
*   
* 
*   SOLID 원칙을 잘 순하고 있나?
*   SRP(단일 책임 원칙) -> 클래스 명으로 추측하면 상점 클래스인데 판매가 아닌 레시피 제작도 기능도 소유
*       -> 분리 필요
* 
*   OCP(개방 페쇄 원칙) -> 상점클래스에서 제작과 판매를 동시에 하려고 하면, 코드변경 and 확장시 해당 클래스 전체에 영향을 미침
        -> 분리 필요
*
*   LSP(리스코프 치환 원칙 - 부모의 의도대로 자식 작동)
*   ISP (인터페이스 분리 원치) - 인터페이스 세분화
*   DIP (의존 역전 원칙) - 고수준 모듈에서의 추상화하여 저수준에 의존 x 
*       -> 전체적으로 인터페이스를 사용하고 있지 않은 강결합 상태이다.
*   
*/

/*
* 
* typeid
* decltype
* noexcept
* stl 시리즈
*/


// PotionRecipe 클래스: 재료 목록을 vector<string>으로 변경
class PotionRecipe
{
public:
    std::string potionName;
    std::vector<std::string> ingredients; // 단일 재료에서 재료 '목록'으로 변경

    // 생성자: 재료 목록을 받아 초기화하도록 수정
    PotionRecipe(const std::string& name, const std::vector<std::string>& ingredients)
        : potionName(name), ingredients(ingredients)
    {
    }

    void PrintRecipe() const
    {
        std::cout << "- 물약 이름: " << potionName << std::endl;
        std::cout << "  > 필요 재료: ";

        // 재료 목록을 순회하며 출력
        for (size_t j = 0; j < ingredients.size(); ++j)
        {
            std::cout << ingredients[j];
            // 마지막 재료가 아니면 쉼표로 구분
            if (j < ingredients.size() - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
};

// AlchemyWorkshop 클래스: 레시피 목록을 관리
class AlchemyWorkshop 
{
private:
    // 소유 레시피 목록
    std::vector<PotionRecipe> recipes;

public:
    // addRecipe 메서드: 재료 목록(vector)을 매개변수로 받도록 수정
    void addRecipe(const std::string& name, const std::vector<std::string>& ingredients) {
        recipes.push_back(PotionRecipe(name, ingredients));
        std::cout << ">> 새로운 레시피 '" << name << "'이(가) 추가되었습니다." << std::endl;
    }

    // 모든 레시피 출력 메서드
    void displayAllRecipes() const
    {
        if (recipes.empty()) {
            std::cout << "아직 등록된 레시피가 없습니다." << std::endl;
            return;
        }

        std::cout << "\n--- [ 전체 레시피 목록 ] ---" << std::endl;
        for (const auto& recipe : recipes)
        {
            recipe.PrintRecipe();
        }
        std::cout << "---------------------------\n";
    }

    std::optional<PotionRecipe> searchRecipeByName(const std::string& name)
    {
        auto iter = std::find_if(recipes.begin(), recipes.end(), [name](const PotionRecipe& Potion) {
            return Potion.potionName == name;
            });

        // 레시피가 없는 경우
        if (iter == recipes.end())
        {
            return std::nullopt;
        }
        
        return *iter;
    }

    std::vector<PotionRecipe> searchRecipeByIngredient(const std::string& ingredient)
    {
        std::vector<PotionRecipe> findRecipes;
        for (const auto& recipe : recipes)
        {
            for (const auto& recipeIngredient : recipe.ingredients)
            {
                if (ingredient == recipeIngredient)
                {
                    findRecipes.push_back(recipe);
                    break;
                }
            }
        }

        return findRecipes;
    }
};

int main()
{
    AlchemyWorkshop myWorkshop;

    while (true) 
    {
        std::cout << "⚗️ 연금술 공방 관리 시스템" << std::endl;
        std::cout << "1. 레시피 추가" << std::endl;
        std::cout << "2. 모든 레시피 출력" << std::endl;
        std::cout << "3. 특정 이름 레시피 찾기" << std::endl;
        std::cout << "4. 특정 재료 포함 레시피 찾기" << std::endl;
        std::cout << "5. 종료" << std::endl;
        std::cout << "선택: ";

        int choice;
        std::cin >> choice;

        if (std::cin.fail()) 
        {
            std::cout << "잘못된 입력입니다. 숫자를 입력해주세요." << std::endl;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 1) 
        {
            std::string name;
            std::cout << "물약 이름: ";
            std::cin.ignore(10000, '\n');
            std::getline(std::cin, name);

            // 여러 재료를 입력받기 위한 로직
            std::vector<std::string> ingredients_input;
            std::string ingredient;
            std::cout << "필요한 재료들을 입력하세요. (입력 완료 시 '끝' 입력)" << std::endl;

            while (true)
            {
                std::cout << "재료 입력: ";
                std::getline(std::cin, ingredient);

                // 사용자가 '끝'을 입력하면 재료 입력 종료
                if (ingredient == "끝") {
                    break;
                }
                ingredients_input.push_back(ingredient);
            }

            // 입력받은 재료가 하나 이상 있을 때만 레시피 추가
            if (!ingredients_input.empty()) {
                myWorkshop.addRecipe(name, ingredients_input);
            }
            else 
            {
                std::cout << ">> 재료가 입력되지 않아 레시피 추가를 취소합니다." << std::endl;
            }

        }
        else if (choice == 2) 
        {
            myWorkshop.displayAllRecipes();

        }
        else if (choice == 3)
        {
            std::string recipeName;
            std::cin.ignore(10000, '\n');
            std::cout << "레시피 찾기. 레시피 이름을 입력하세요 : ";
            std::getline(std::cin, recipeName);

            std::optional<PotionRecipe> FindedRecipe = myWorkshop.searchRecipeByName(recipeName);
            if (FindedRecipe.has_value())
            {
                FindedRecipe->PrintRecipe();
            }
            else
            {
                std::cout << "레시피가 존재하지 않습니다." << std::endl;
            }
        }
        else if (choice == 4)
        {
            std::string ingredient;
            std::cin.ignore(10000, '\n');
            std::cout << "재료 포함된 레시피 찾기. 재료를 입력하세요 : " << std::endl;
            std::getline(std::cin, ingredient);

            const auto& FindedRecipes = myWorkshop.searchRecipeByIngredient(ingredient);
            if (FindedRecipes.empty())
            {
                std::cout << "해당 재료가 포함된 레시피가 존재하지 않습니다." << std::endl;
            }
            else
            {
                for (const auto& Recip : FindedRecipes)
                {
                    Recip.PrintRecipe();
                }
            }
        }
        else if (choice == 5) 
        {
            std::cout << "공방 문을 닫습니다..." << std::endl;
            break;

        }
        else 
        {
            std::cout << "잘못된 선택입니다. 다시 시도하세요." << std::endl;
        }
    }

    return 0;
}

