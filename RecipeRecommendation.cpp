#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class Ingredient {
public:
    string name, unit;
    float quantity;
    Ingredient *next;

    Ingredient(string a, string b, float c) : name(a), unit(b), quantity(c), next(NULL) {}

    // Destructor for Ingredient to free memory of the linked list
    ~Ingredient() {
        delete next; // This will recursively delete all following ingredients
    }
};

// Function to add an ingredient to the linked list
void addIngredient(Ingredient *&head, string a, string b, float c) {
    Ingredient *newIngredient = new Ingredient(a, b, c);
    if (head == NULL) {
        head = newIngredient;
    } else {
        Ingredient *temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newIngredient;
    }
}

class Recipe {
public:
    string name, instructions;
    float rating;
    int numberOfRatings;
    Ingredient *ingredients; // Head pointer for the linked list of ingredients
    Recipe *next;

    void addRating(float newRating) {
        numberOfRatings++;
        rating = ((rating * (numberOfRatings - 1)) + newRating) / numberOfRatings; // Update the rating
    }

    Recipe(string a, string b, float c = 0, int d = 0)
        : name(a), instructions(b), rating(c), numberOfRatings(d), ingredients(NULL), next(NULL) {
        // Empty constructor body
    }

    // Destructor for Recipe to free ingredients and next recipes
    ~Recipe() {
        delete ingredients; // This will call the Ingredient destructor
    }

    bool matchesIngredients(Ingredient *availableIngredients) {
        Ingredient *recipeIngredient = ingredients;
        while (recipeIngredient != NULL) {
            Ingredient *available = availableIngredients;
            bool found = false;
            while (available != NULL) {
                if (recipeIngredient->name == available->name &&
                    recipeIngredient->unit == available->unit &&
                    recipeIngredient->quantity <= available->quantity) {
                    found = true;
                    break;
                }
                available = available->next;
            }
            if (!found) {
                return false; // Ingredient not found or insufficient quantity
            }
            recipeIngredient = recipeIngredient->next;
        }
        return true; // All ingredients matched
    }
};

class RecipeDatabase {
public:
    Recipe *head;

    RecipeDatabase() : head(NULL) {}

    void addRecipe(string name, string instructions, float rating, int numberOfRatings) {
        Recipe *newRecipe = new Recipe(name, instructions, rating, numberOfRatings);
        if (head == NULL) {
            head = newRecipe;
        } else {
            Recipe *temp = head;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = newRecipe;
        }
        cout << "Recipe added successfully." << endl;
    }

    void removeRecipe(string recipeName) {
        Recipe *current = head;
        Recipe *previous = NULL;

        // Search for the recipe to remove
        while (current != NULL && current->name != recipeName) {
            previous = current;
            current = current->next;
        }

        // If the recipe was not found
        if (current == NULL) {
            cout << "Recipe not found." << endl;
            return;
        }

        // If the recipe to remove is the head of the list
        if (previous == NULL) {
            head = current->next;
        } else {
            previous->next = current->next;
        }

        // Deallocate memory for the removed recipe
        delete current;
        cout << "Recipe removed successfully." << endl;
    }

    Recipe *findRecipeByName(const string& recipeName) {
        Recipe *current = head;
        while (current != NULL) {
            if (current->name == recipeName) {
                return current; // Return the matching recipe by name
            }
            current = current->next;
        }
        return NULL; // No matching recipe found
    }

    Recipe *findRecipe(Ingredient *availableIngredients) {
        Recipe *current = head;
        while (current != NULL) {
            if (current->matchesIngredients(availableIngredients)) {
                return current; // Return the matching recipe
            }
            current = current->next;
        }
        return NULL; // No matching recipe found
    }

    void loadRecipes(const string &filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Could not open the file!" << endl;
            return;
        }

        string name, instructions;
        float rating;
        int numberOfRatings;

        while (file >> ws && getline(file, name) && getline(file, instructions) >> rating >> numberOfRatings) {
            addRecipe(name, instructions, rating, numberOfRatings);
        }
        file.close();
    }

    void saveRecipes(const string &filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Could not open the file for writing!" << endl;
            return;
        }

        Recipe *current = head;
        while (current != NULL) {
            file << current->name << endl;
            file << current->instructions << endl;
            file << current->rating << " " << current->numberOfRatings << endl;
            current = current->next;
        }
        file.close();
    }
};

class RecipeSuggestionSystem {
private:
    RecipeDatabase recipeDB; // Instance of RecipeDatabase

public:
    // Method to get a suggestion based on available ingredients
    Recipe *getSuggestion(Ingredient *availableIngredients) {
        return recipeDB.findRecipe(availableIngredients); // Find a recipe that matches the available ingredients
    }

    // Method to access RecipeDatabase
    RecipeDatabase& getRecipeDatabase() {
        return recipeDB;
    }
};

class UserInterface {
private:
    RecipeSuggestionSystem system; // Instance of RecipeSuggestionSystem

public:
    // Function to input ingredients
    Ingredient* inputIngredients() {
        Ingredient *ingredients = NULL;
        string name, unit;
        float quantity;
        int addMore;

        do {
            cout << "Enter ingredient details (name unit quantity): ";
            cin >> name >> unit >> quantity;
            if (quantity < 0) {
                cout << "Quantity cannot be negative. Please try again." << endl;
                continue; // Skip this iteration if quantity is invalid
            }
            addIngredient(ingredients, name, unit, quantity);
            cout << "Enter 0 to stop adding ingredients or any other number to continue: ";
            cin >> addMore;
        } while (addMore != 0);
        return ingredients; // Return the list of ingredients
    }

    // Function to display recipe suggestions
    void displaySuggestions(Recipe* recipes) {
        if (recipes == NULL) {
            cout << "No recipes found." << endl;
            return;
        }

        cout << "Suggested Recipes:" << endl;
        while (recipes != NULL) {
            cout << "- " << recipes->name << ": " << recipes->instructions << " (Rating: " << recipes->rating << ")" << endl;
            recipes = recipes->next;
        }
    }

    // Function to run the user interface
    void run() {
        // Load recipes from file
        system.getRecipeDatabase().loadRecipes("recipes.txt");

        Ingredient *availableIngredients = inputIngredients(); // Input available ingredients

        // Get suggestions
        Recipe* suggestedRecipe = system.getSuggestion(availableIngredients);
        displaySuggestions(suggestedRecipe);

        // Input leftovers and get suggestions
        Ingredient *leftovers = inputIngredients(); // Assume this is for leftovers
        Recipe* leftoverSuggestions = system.getSuggestion(leftovers);
        displaySuggestions(leftoverSuggestions);

        // Rate a recipe
        string recipeName;
        float rating;
        cout << "Enter the recipe name you want to rate: ";
        cin.ignore(); // Clear input buffer
        getline(cin, recipeName);
        cout << "Enter your rating (0 to 5): ";
        cin >> rating;

        // Check if the rating is within the valid range
        if (rating < 0 || rating > 5) {
            cout << "Rating must be between 0 and 5." << endl;
            return;
        }

        Recipe *recipeToRate = system.getRecipeDatabase().findRecipeByName(recipeName); // Find the recipe to rate
        if (recipeToRate != NULL) {
            recipeToRate->addRating(rating); // Update the recipe rating
            cout << "Thank you for rating " << recipeName << " with " << rating << " stars!" << endl;
        } else {
            cout << "Recipe not found for rating." << endl;
        }

        // Save recipes to file
        system.getRecipeDatabase().saveRecipes("recipes.txt");

        // Cleanup
        delete availableIngredients; // Free memory for available ingredients
        delete leftovers; // Free memory for leftovers
    }
};

int main() {
    UserInterface ui;
    ui.run(); // Start the user interface
    return 0;
}
