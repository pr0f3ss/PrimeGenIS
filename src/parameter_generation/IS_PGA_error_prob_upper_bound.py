# Courtesy of Mia Filic and her team

#%%
# Due to Brandt /Damgard paper we are able to upper bound the number of rounds needed for the IS PGA (MRt) algorithm to reach certain error probability.

import numpy as np

import xlsxwriter,datetime,os


def qkts(k=1024, L=1024*np.log(2)*10, t=1, M = -1):

    Cm_sum = []

    for m in range(3,M+1):

        Cm_sum.append(8.0/3.0*(np.pi**2 -6.0)*np.sum([np.power(2, m-j-(k-1.0)/j)for j in range(2,m+1)])*(2**(-t*(m-1))))

    

    c = L/(k*np.log(2))

    q = 0.5*(c*k)**2 * np.sum(Cm_sum) + 0.7*c*k*(2**(-t*M))

    return q


timestamp = datetime.datetime.now()

my_date = f"{timestamp:%Y-%m-%d %H:%M:%S}"

c_list=[0.5,1,5,10,15,20,25,30,35,70]

k_list = [100,150,200,500,512,550,600,1024,2048]

t_list=[i for i in range(1,20)]

L_list = [L for L in range(4000,65536,512)]

#%%

dir = 'data\\'

k=1024

#%%

# Create an new Excel file and add a worksheet.

workbook = xlsxwriter.Workbook('data\\qktsdata.xlsx', {'nan_inf_to_errors': True})

worksheet = workbook.add_worksheet()


# Widen the first column to make the text clearer.

worksheet.set_column('A:A', 20)


# Add a bold format to use to highlight cells.

bold = workbook.add_format({'bold': True})

worksheet.write(0,0, 't \ L')

#%%

for j,t in enumerate(t_list):

    worksheet.write(j+1,0, str(t))


    for i,L in enumerate(L_list):

        M_max = int(2*np.sqrt(k-1)-1)+1

        q_list = [qkts(k,L,t,M) for M in range(M_max)]

        M_min = np.where(q_list == np.min(q_list))

        p_inc_search_log2 = np.log2(q_list[M_min[0][0]])

        worksheet.write(0,i+1, L)

        worksheet.write(j+1,i+1, -p_inc_search_log2)

workbook.close()

#%%