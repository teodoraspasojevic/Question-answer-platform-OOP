# Question-answer-platfrom-oop
Implementation of a part of a question and answer platform using the object-oriented programming and a tree as a data structure, with the level order algorithm for search through the platform.

Implementation

The platform containing questions and different answers is implemented in an object-oriented way. Firstly, we have the abstract class TreeNode, from which two different types of TreeNodes that would be contained in the platform are derived: Questions and Answers. We use the abstract class TreeNode so we can create a class Tree that will contain Questions and Answers, but both objects can be saved under the pointer for the TreeNode class. We derive two different classes, Question and Answer, so that we can add additional functionalities to the Answer class, such as a new attribute "star," which the Question class cannot have.

The next class we implemented is the Tree class, which has a pointer to the root TreeNode. The Tree class is implemented so we can store a Question and different new Questions and Answers to it. In this class, we implement all of the methods that are necessary for searching through the Tree. For tree search, the assignment requires us to use only the level order algorithm.

The final class is the QuestionPlatform class, which has a vector of pointers to different initial Questions. This class stores all of the Questions and all of the reactions to them. 

Moreover, exception handling is implemented using try-catch blocks and creating custom exception classes.
