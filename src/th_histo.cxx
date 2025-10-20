#include "th_histo.h"
#include <TFile.h>

void th_histo::save(TFile *root_file, const std::string &histo_name) const
{
    for (int i = 0; i < correct.size(); ++i)
    {
        if (correct[i])
        {
            correct[i]->SetName((histo_name + "_correct_" + std::to_string(i)).c_str());
            correct[i]->Write();
        }
    }
    for (int i = 0; i < wrong.size(); ++i)
    {
        if (wrong[i])
        {
            wrong[i]->SetName((histo_name + "_wrong_" + std::to_string(i)).c_str());
            wrong[i]->Write();
        }
    }

    for (size_t i = 0; i < forward.size(); ++i)
    {
        if (forward[i])
        {
            forward[i]->SetName((histo_name + "_forward_" + std::to_string(i)).c_str());
            forward[i]->Write();
        }
    }
    for (size_t i = 0; i < backward.size(); ++i)
    {
        if (backward[i])
        {
            backward[i]->SetName((histo_name + "_backward_" + std::to_string(i)).c_str());
            backward[i]->Write();
        }
    }
    if (total)
    {
        total->SetName((histo_name + "_total").c_str());
        total->Write();
    }
}